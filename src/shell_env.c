#include "../inc/ush.h"

extern char **environ;

char *delete_qte(char *src) 
{
    char *temp = mx_strdup(src);
    int quot_num = 0;
    int i = 0;
    while(i < mx_strlen(temp))
    {
        if ((temp[i] == '\'' && temp[i - 1] != '\\')||(temp[i] == '\"'&& temp[i - 1] != '\\')) {
            quot_num++;
        }
        i++;   
    }

    if (!quot_num) 
    {
        return temp;
    }
    int size = mx_strlen(temp) - quot_num;
    char *resrce = mx_strnew(size);
    int k = 0;
    i = 0;
    while(i < mx_strlen(temp))
    {
        if (temp[i] != '\"' && temp[i] != '\'') {
            resrce[k] = temp[i];
            k++;
        }
        i++;
    }
    return resrce;
}

static int parse_env(t_list *arguments, bool *i_flag, bool *u_flag, bool *P_flag, 
    t_list **u_parameter, t_list **p_parameter, t_list **prog_arguments) 
{
    char **array = NULL;
    t_list *list = arguments;
    array = (char **)malloc((mx_list_size(list) + 1) * sizeof(char *));
    int k = 0;
    while (list) {
        (array)[k] = mx_strdup(list->data);
        k++;
        list = list -> next;
    }
    array[k] = NULL;

    int i = 0;


    while(i < mx_list_size(arguments))
    {
        bool cur_u = false;
        bool cur_i = false;
        bool cur_P = false;

        if (array[i][0] == '-') {
            int j = 1;
            while(j<mx_strlen(array[i]))
            {
                if (array[i][j] == 'u') {
                    if (!cur_i) 
                    {
                        *u_flag = true;
                        cur_u = true;
                        if (!array[i + 1]) 
                        {
                            *u_flag = false;
                            mx_printerr("env: option requires an argument -- u\n");
                            return -1;
                        }
                        mx_push_back(u_parameter, mx_strdup(array[i + 1]));
                        i++;
                        break;
                    }
                }
                else if (array[i][j] == 'i') {
                    if (!(cur_u)) {
                        cur_i = true;
                        *i_flag = true;
                    }
                    else {
                        cur_u = false;
                        *u_flag = false;
                    }
                }
                else if(array[i][j] == 'P' && !cur_u) {
                    cur_P = true;
                    *P_flag = true;
                    if (!array[i + 1]) {
                        *P_flag = false;
                        mx_printerr("env: option requires an argument -- P\n");
                        return -1;
                    }
                    mx_push_back(p_parameter, mx_strdup(array[i + 1]));
                    i++;
                    break;
                }
                else {
                    while (i < mx_list_size(arguments)) {
                        mx_strdel(&array[i]);
                        i++;
                    }
                    return -1;
                }
                j++;
            }
        }
        if (array[i][0] != '-' && (array[i - 1][0] != '-' || *i_flag) && i < mx_list_size(arguments)) {
            while (i < mx_list_size(arguments)) {
                mx_push_back(prog_arguments, mx_strdup(array[i]));
                mx_strdel(&array[i]);
                i++;
            }
            break;
        } 
        i++;
    }
    for(int i = 0; i < 10; i++)
    {

    }
    return 0;
}

int launch_proc_env(char **arguments, char **env, char *path, t_our_ush *ush) {
    pid_t pid;
    int stat = 0;
    extern char **environ;
    pid = fork();
    canon_disable();
    if (pid == 0) 
    {
        // Дочерний процесс
        sigset_t x;
        sigemptyset (&x);
        sigaddset(&x, SIGINT);
        sigaddset(&x, SIGSTOP);
        sigaddset(&x, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &x, NULL);
        char *programm = get_prog_path(arguments[0], path);

        if (!programm) {
            mx_printerr("env: ");
            mx_printerr(arguments[0]);
            mx_printerr(": No such file or directory\n");
            exit(127);
        }
        if (execve(programm, arguments, env) == -1) {
            mx_printerr("ush: command not found: ");
            mx_printerr(arguments[0]);
            mx_printerr("\n");
            mx_strdel(&programm);
            exit(127);
        }

        mx_strdel(&programm);
        exit(0);
    } 
    else if (pid < 0) 
    {
        perror("ush");
    } else 
    {
        t_process *proc = start_process(pid);
        ush->frontend = proc;
        if (waitpid(pid, &stat, WUNTRACED) != -1) 
        {
            if (WIFSTOPPED(stat)) 
            {
                proc->stat = stat;
                ush->frontend = NULL;
                add_backgr(proc, ush);
                
            }
        }
        if (!WIFSTOPPED(stat)) 
        {
            tcsetpgrp(STDIN_FILENO, getpgrp());  
            canon_enable();
        }
   }

    return stat >> 8;
}

void mx_env(t_our_ush *ush) {
    bool u_flag = false;
    bool P_flag = false;
    bool i_flag = false;

    t_list *p_parameter= NULL;
    t_list *u_parameter = NULL;
    t_list *prog_arguments = NULL;

    int resrce = parse_env(ush->arguments, &i_flag, &u_flag, &P_flag, &u_parameter, &p_parameter, &prog_arguments);

    if (resrce == -1) {
        mx_printerr("ush: env [-i] [-P utilpath] [-u name]\n");
        ush->error = 1;
        return;
    } 
    char **arguments = NULL;
    arr_from_list(ush->arguments, &arguments);

    char **array_u = NULL;
    char *realpath = NULL;
    if (p_parameter) {
        char **p_arr = NULL;
        arr_from_list(p_parameter, &p_arr);
        realpath = mx_strdup(p_arr[0]);

        /*
        for (int i = 1; i < mx_list_size(p_parameter); i++) 
        {
            char *temp1 = mx_strjoin(realpath, ":");
            mx_strdel(&realpath);
            realpath = mx_strjoin(temp1, p_arr[i-1]);
        }
        */

        for (int i = 1; i < mx_list_size(p_parameter); i++) 
        {
            char *temp = mx_strjoin(realpath, ":");
            mx_strdel(&realpath);
            realpath = mx_strjoin(temp, p_arr[i]);
        }
    }
    if (!realpath) {
        char *env_u = getenv("PATH");
        if (env_u)
            realpath = mx_strdup(env_u);
    }
    if (u_parameter)
        arr_from_list(u_parameter, &array_u);

    t_list *envlist = NULL;
    int var_amount = 0;
    if (u_flag) {
        int i = 0;
        for (; environ[i]; i++) {
                bool compare = false;
                int j = 0;
                while(j < mx_list_size(u_parameter))
                {
                    if (mx_strncmp(environ[i], array_u[j], mx_strlen(array_u[j])) == 0 
                    && environ[i][mx_strlen(array_u[j])] == '=')
                    {
                        compare = true;
                    }
                    j++;
                }
            if (compare) {
                continue;
            }
            mx_push_back(&envlist, environ[i]);
        }
        var_amount = i;
    }

    char **array_env = NULL;
    t_list *tmplist = envlist;
    array_env = (char **)malloc((mx_list_size(tmplist)+1) * sizeof(char *));
    int d = 0;
    while (tmplist!=NULL) {
        (array_env)[d] = mx_strdup(tmplist->data);
        d++;
        tmplist = tmplist -> next;
    }
    array_env[d] = NULL;
    mx_clear_list(&envlist);
    if (prog_arguments) {
        char **arguments = NULL;
        arr_from_list(prog_arguments, &arguments);
        if (realpath) {
            if (i_flag) {
                ush->error = launch_proc_env(arguments, NULL, realpath, ush);
            }
            else {
                ush->error = launch_proc_env(arguments, array_env, realpath, ush);
            }
        }
        else {
            mx_printerr("env: command not found: ");
            mx_printerr(arguments[0]);
            mx_printerr("\n");
            ush->error = 127;
            return;
        }
    }
    else {
        if (var_amount) {
            for (int i = 0; i < var_amount; i++) {
                if (array_env[i]) {
                    printf("%s\n", array_env[i]);
                }
            }
        }
        else {
            if (!i_flag)
                for (int i = 0; environ[i]; i++) { 
                    printf("%s\n", environ[i]);
                }
        }
    }
    mx_del_strarr(&array_env);
    mx_strdel(&realpath);
    ush->error = 0;
}
