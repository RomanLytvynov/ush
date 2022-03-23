#include "../inc/ush.h"

char *get_prog_path(char *name, const char *PATH) {
    struct stat buf;
    if (stat(name, &buf) == 0) {
        return name;
    }
    
    char **arr_path = mx_strsplit(PATH, ':');
    if (!arr_path) {
        return NULL;
    }
    for (int i = 0; arr_path[i]; i++) {
        DIR *cur_dir = opendir(arr_path[i]);
        if (cur_dir) {
            struct dirent *temp = readdir(cur_dir);
            while (temp) {
                //////////////
                if (mx_strcmp(name, temp->d_name) == 0) {
                    char *temp_path = mx_strjoin(arr_path[i], "/");
                    char *path = mx_strjoin(temp_path, temp->d_name);
                    mx_strdel(&temp_path);
                    closedir(cur_dir);
                    return path;
                }
                /////////////////
                temp = readdir(cur_dir);
            }
        }    
    }

    return NULL;
}

void ush_init(t_our_ush **ush) {

    *ush = (t_our_ush *)malloc(sizeof(t_our_ush));
    
    
    (*ush)->input = NULL;
    (*ush)->num_cmd = -2;
    (*ush)->arguments = NULL;
    (*ush)->error = 0;
    (*ush)->qte = false;
    (*ush)->pid = getpid();
    (*ush)->pgid = getpgid((*ush)->pid);
    (*ush)->count_backgrounds = 0;

    (*ush)->cmd_queue = NULL;

    (*ush)->frontend = NULL;
    (*ush)->backend = NULL;
    (*ush)->arguments_arr = NULL;
    (*ush)->make_redirection = 0;

     char cwd[PATH_MAX];

    if (getenv("HOME") == NULL) {
        struct passwd *pw = getpwuid(getuid());
        
        setenv("HOME", pw->pw_dir, 1);
    }
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        setenv("PWD", cwd, 1);
    if (getenv("OLDPWD") != NULL) {
        unsetenv("OLDPWD");
    }
    if (getenv("PATH") == NULL)
        setenv("PATH", "/bin:/usr/bin:/usr/ucb:/usr/local/bin", 1);
    if (getenv("SHLVL") == NULL)
        setenv("SHLVL", "1", 1);
    else {
        char *shlvl = mx_itoa(mx_atoi(getenv("SHLVL")) + 1);
        setenv("SHLVL", shlvl, 1);
        mx_strdel(&shlvl);
    }
    setenv("_", "/usr/bin/env", 1);
    (*ush) -> pwd = mx_strdup(getenv("PWD"));
    (*ush) -> prev_pwd = NULL;
    sigfillset(&(*ush)->mask);

    sigprocmask(SIG_SETMASK, &(*ush)->mask, NULL);
    tcgetattr(STDIN_FILENO, get_tty());
    setvbuf(stdout, NULL, _IONBF, 0);
    canon_enable();
}

int launch_proc(char **argv, t_our_ush *ush) {
    
    pid_t pid;
    int stat = 0;
    extern char **environ;
    pid = fork();
    canon_disable();
    if (pid == 0) {
        sigset_t x;
        sigemptyset (&x);
        sigaddset(&x, SIGINT);
        sigaddset(&x, SIGSTOP);
        sigaddset(&x, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &x, NULL);

        char *prog = get_prog_path(argv[0], getenv("PATH"));

        bool is_path = false;
        if (mx_strchr(argv[0], '/')) {
            is_path = true;
        }

        if (!prog && is_path) {
            mx_printerr("ush: no such file or directory: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            exit(127);
        }

        if (execve(prog, argv, environ) == -1) {
            mx_printerr("ush: command not found: ");
            mx_printerr(argv[0]);
            mx_printerr("\n");
            exit(127);
        }
        exit(0);
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("ush");
    } else {
        t_process *proc = start_process(pid);
        ush->frontend = proc;
        if (waitpid(pid, &stat, WUNTRACED) != -1) {
            if (WIFSTOPPED(stat)) {
                proc->stat = stat;
                ush->frontend = NULL;
                add_backgr(proc, ush);
            }
        }
        if (!WIFSTOPPED(stat)) {
            free(proc);
            proc = NULL;
            tcsetpgrp(STDIN_FILENO, getpgrp());
            canon_enable();
        }
    }
    
    return stat >> 8;
}

void arr_from_list(t_list *list, char ***arr) {
    *arr = (char **)malloc((mx_list_size(list)) * sizeof(char *));
    int i = 0;
    while (list) {
        (*arr)[i] = mx_strdup(list->data);
        i++;
        list = list -> next;
    }
}

void canon_disable(void) {
    tcsetattr(STDIN_FILENO, TCSADRAIN, get_tty());
}

void canon_enable(void) {
    struct termios tty;

    tcgetattr(STDIN_FILENO, get_tty());
    tty = *get_tty();
    tty.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &tty);
}
