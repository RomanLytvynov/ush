#include "../inc/ush.h"

void mx_cd(t_our_ush *ush) {
    bool P_flag = false;
    bool s_flag = false;

    t_list *check_opt = ush->arguments;
    char **arguments = NULL;

    if (check_opt) 
    {
        arr_from_list(check_opt, &arguments);
        int i = mx_list_size(check_opt) - 1;
        while(i >= 0)
        {
            if (arguments[i][0] == '-' && arguments[i][1] != '/')
            {
                int j = mx_strlen(arguments[i]) - 1;
                while(j > 0)
                {
                        if (arguments[i][0] == '-' && arguments[i][j] != 's' && arguments[i][j] != 'P') {
                        mx_printerr("u$h: cd: -");
                        mx_printerr(&arguments[i][j]);
                        mx_printerr(": invalid option\n");
                        mx_printerr("cd: usage: cd [-s] [-P] [dir]\n");
                        ush->error = 1;
                        return;
                    }
                    if (arguments[i][j] == 's' && arguments[i][0] == '-') {
                        s_flag = true;
                    }
                    if (arguments[i][j] == 'P' && arguments[i][0] == '-') {
                        P_flag = true;
                    }
                    j--;
                }
                mx_strdel(&arguments[i]);
                arguments[i] = NULL;
            }
            i--;
        }
        mx_strdel(&arguments[0]);
        arguments[0] = NULL;
        free(arguments);
        arguments = NULL;
    }
    //go to the home directory
    if (!ush->arguments) 
    {
        if (malloc_size(ush -> prev_pwd) != 0)
            mx_strdel(&ush->prev_pwd);
        ush->prev_pwd = mx_strdup(ush->pwd);
        if (malloc_size(ush -> pwd) != 0)
            mx_strdel(&ush->pwd);
        ush->pwd = getenv("HOME");
        chdir(ush->pwd);
        return;
    }
    //try to go in more than two directories
    if (mx_list_size(ush->arguments) > 2) 
    {
        mx_printerr("cd: too many arguments\n");
        ush->error = 1;
        return;
    }

    while (((char *)ush->arguments->data)[0] == '-' && ((char *)ush->arguments->data)[1] != '/' 
        && mx_strcmp(ush->arguments->data, "-") != 0) {
        mx_pop_front(&ush->arguments);
    }
    //if home directory pionts
    if (!mx_strcmp(ush->arguments->data,".")) {
        return;
    }

    /*if (mx_strcmp(ush->arguments->data, "/") == 0) {
        if (malloc_size(ush -> pwd) != 0)
            mx_strdel(&ush->prev_pwd);
        ush->prev_pwd = mx_strdup(ush->pwd);
        if (malloc_size(ush -> prev_pwd) != 0)
            mx_strdel(&ush->pwd);
        ush->pwd = mx_strdup("/");
        chdir(ush->pwd);
        return;
    }*/

    if (mx_strcmp(ush->arguments->data, "/") == 0) {
        if (malloc_size(ush -> prev_pwd) != 0)
            mx_strdel(&ush->prev_pwd);
        ush->prev_pwd = mx_strdup(ush->pwd);
        if (malloc_size(ush -> pwd) != 0)
            mx_strdel(&ush->pwd);
        ush->pwd = mx_strdup("/");
        chdir(ush->pwd);
        return;
    }
    //whether lnk or dir
    if (s_flag) {
        char *dir = realpath(ush->arguments->data, NULL);
        if (dir && mx_strcmp(dir, ush->arguments->data) != 0) {
            mx_printerr("ush: cd: ");
            mx_printerr(ush->arguments->data);
            mx_printerr(": Not a directory\n");
        }
    }

    if (mx_strcmp(ush->arguments->data,"-") == 0) {
        if (getenv("OLDPWD") == NULL) {
            mx_printerr("ush: cd: OLDPWD not set\n");
            ush -> error = 1;
            return;
        }
        char *tmp = ush->prev_pwd;
        ush->prev_pwd = ush->pwd;
        ush->pwd = tmp;
        chdir(ush->pwd);
        
        setenv("PWD", ush->pwd, 1);
        setenv("OLDPWD", ush->prev_pwd, 1);

        return;
    }

    if (((char *)ush->arguments->data)[0] == '-' && ((char *)ush->arguments->data)[1] == '/') {
       char *tmp = mx_strjoin(getenv("OLDPWD") ,((char *)ush->arguments->data) + 1);
        char *to_delete = ush->arguments->data;
        mx_strdel(&(to_delete));
        ush->arguments->data = tmp;
    }

    char *cuur_pwd = mx_strdup(ush->arguments->data);
    if (cuur_pwd[mx_strlen(cuur_pwd) - 1] == '/') {
        cuur_pwd[mx_strlen(cuur_pwd) - 1] = '\0';
    }
    char *pwd_tmp = mx_strdup(ush -> pwd);
    if (((char *)(ush->arguments->data))[0] != '/') {
        mx_strdel(&cuur_pwd);
        char *real_path = ush->arguments->data;
        char **path = mx_strsplit(real_path,'/');
        int i = 0;
        while(path[i])
        {
            if (mx_strcmp(path[i], "..") == 0) {
                char *tmp = pwd_tmp;
                while (mx_strchr(tmp, '/')) {
                    tmp = mx_strchr(tmp, '/') + 1;
                }
                mx_strdel(&cuur_pwd);
                cuur_pwd = mx_strndup(pwd_tmp, mx_strlen(pwd_tmp) - mx_strlen(tmp) - 1);
            }
            else if (mx_strcmp(path[i], ".") == 0){
                continue;
            }
            else {
                char *tmp = mx_strdup(pwd_tmp);
                if (pwd_tmp[mx_strlen(pwd_tmp) - 1] != '/' && mx_strlen(pwd_tmp) != 0) {
                    mx_strdel(&tmp);
                    tmp = mx_strjoin (pwd_tmp, "/");
                }
                cuur_pwd = mx_strjoin(tmp, path[i]);
                mx_strdel(&tmp);
            }
            mx_strdel(&pwd_tmp);
            pwd_tmp = mx_strdup(cuur_pwd);
            mx_strdel(&path[i]);
            path[i] = NULL;
            i++;
        }
        free(path);
        path=NULL;

        if (mx_strlen(cuur_pwd) == 0) {
            mx_strdel(&cuur_pwd);
            cuur_pwd = mx_strdup("/");
        }
    }

    struct stat temp_stat;
    if (stat(cuur_pwd, &temp_stat) == 0)
    {
        if (!S_ISDIR(temp_stat.st_mode)) 
        {
            mx_printerr("cd: not a directory: ");
            mx_printerr(ush->arguments->data);
            mx_printerr("\n");
            ush -> error = 1;
            return;
        }
    }
    else 
    {
        mx_printerr("cd: no such file or directory: ");
        mx_printerr(ush->arguments->data);
        mx_printerr("\n");
        ush -> error = 1;
        return;
    }
    char *tmp_path = mx_strjoin(cuur_pwd, "/.");
    if(lstat(tmp_path, &temp_stat) == -1) 
    {
        if (errno == 13) 
        {
            mx_printerr("cd: permission denied: ");
            mx_printerr(ush->arguments->data);
            mx_printerr("\n");
            errno = 0;
            ush -> error = 1;
            return;
        }
    }

    if (malloc_size(ush -> prev_pwd) != 0)
    {
        mx_strdel(&ush->prev_pwd);
    }
    ush->prev_pwd = mx_strdup(ush->pwd);
    setenv("OLDPWD",ush->pwd,1);
    if (malloc_size(ush -> pwd) != 0)
    {
        mx_strdel(&ush -> pwd);
    }
    ush->pwd = cuur_pwd;
    setenv("PWD",cuur_pwd,1);
    chdir(cuur_pwd);
    
    if (P_flag != false) 
    {
        char *directory = getcwd(NULL, 1024);
        if (directory != NULL) 
        {
            if (malloc_size(ush -> pwd) != 0)
            {
                mx_strdel(&ush->pwd);
            }
            ush->pwd = directory;
            setenv("PWD",directory,1);
            chdir(directory);
        }
    }
    mx_strdel(&tmp_path);
    mx_strdel(&pwd_tmp);
    ush->error = 0;
}
