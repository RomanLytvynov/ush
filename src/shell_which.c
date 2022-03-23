#include "../inc/ush.h"

static int check_include(t_list *l_argv){
    char *include_in[] = {"do", "done", "esac", "then", "elif", "else", "fi",
                            "for", "case", "if", "while", "function", "repeat",
                            "time","until", "select", "coproc", "nocorrect",
                            "foreach", "end", "!", "[", "{", "}"};
    
    for(int i = 0; i < 24; i++){
        if(mx_strcmp(l_argv->data, include_in[i]) == 0){
            mx_printerr(l_argv->data);
            mx_printerr(": ush reserved word\n");
            return 0;
        }
    }
    return 1;
}

static int check_built_in(t_list *l_argv){
    char *MX_BUILT_IN_COMAND[] = {":", ".", "[", "alias", "bg", "bind", "break", "builtin", "case",
 "cd", "command", "compgen", "complete", "continue", "declare", "dirs", "disown", "echo",
  "enable", "eval", "exec", "exit", "export", "fc", "fg", "getopts", "hash", "help", "history",
   "if", "jobs", "kill", "let", "local", "logout", "popd", "printf", "pushd", "pwd", "read",
    "readonly", "return", "set", "shift", "shopt", "source", "suspend", "test", "times", "trap",
  "type", "typeset", "ulimit", "umask", "unalias", "unset", "until", "wait", "while"};

    for(int i = 0; i < 59; i++){
        if(mx_strcmp(l_argv->data, MX_BUILT_IN_COMAND[i]) == 0){
            mx_printerr(l_argv->data);
            mx_printerr(": ush built-in command\n");
            return 0;
        }
    }
    return 1;    
}

static int find_way(char *buff, char **path, bool flag_a, bool flag_s, bool reserv, bool built_in){
    bool is_exist = false;
    char *enviroment_way = mx_strdup(getenv("PATH"));
    
    
    path = mx_strsplit(enviroment_way, ':');
/*

DIR *dir;
    struct dirent *dent;
        for (int j = 0; j < counter; j++ ) {
            if((dir = opendir(ways[j])) != NULL)
            {
                while((dent=readdir(dir))!=NULL) {
                    if(mx_strcmp(argv,dent->d_name) == 0) {
                        
                        mx_printstr(ways[j]);
                        mx_printstr("/");
                        mx_printstr(argv);
                        mx_printstr("\n");
                        if(type == 1){
                            break;
                        }
                    }
                }
            }
        }
        
    closedir(dir);

*/
    DIR *dir;
    struct dirent *dirent;

    for(int j = 0; path[j]; j++){
        dir = opendir(path[j]);
        if(!dir){
            continue;
        }
        while((dirent = readdir(dir)) != NULL){
            if(mx_strcmp(dirent->d_name, buff) == 0 && !reserv && !built_in){
                if(flag_s && !flag_a){
                    mx_del_strarr(&path);
                    mx_strdel(&enviroment_way);
                    return 0;
                }
                else{
                    is_exist = true;
                    mx_printstr(path[j]);
                    mx_printchar('/');
                    mx_printstr(dirent->d_name);
                    mx_printchar('\n');
                    break;
                }
            }
            else if(mx_strcmp(dirent->d_name, buff) == 0 && flag_a){
                if(flag_s && !flag_a){
                    mx_del_strarr(&path);
                    mx_strdel(&enviroment_way);
                    return 0;
                }
                else{
                    is_exist = true;
                    mx_printstr(path[j]);
                    mx_printchar('/');
                    mx_printstr(dirent->d_name);
                    mx_printchar('\n');
                }
                if(mx_strcmp(dirent->d_name, buff) == 0 && !reserv && is_exist && flag_a && built_in){
                    continue;
                }
                else{
                    break;
                }
            }
        }
        closedir(dir);
    }
    /*while (term_arg_cmd[i]) {
        bool built_in = is_builtin_command(term_arg_cmd[i]);
        char *path = find_command_path(term_arg_cmd[i]);
        if (path) {
            int j = mx_get_char_index(path, '/');
            while(mx_get_char_index(&path[j + 1], '/') != -1) {
                j += mx_get_char_index(&path[j + 1], '/') + 1;
            }
            char *cmd = strdup(&path[j + 1]);
            if (built_in) {
                printf("%s: ush built-in cmd\n", cmd);
                if (flags.a && !flags.s) {
                    printf("%s\n", path);
                }
            } else if (!flags.s) {
                printf("%s\n", path);
            }
            free(cmd);
            free(path);*/    mx_del_strarr(&path);
    mx_strdel(&enviroment_way);
    if(!is_exist && !reserv && !built_in){
        mx_printerr(buff);
        mx_printerr(" not found\n");
        return 1;
    }
    return 0;
}


int mx_which(t_our_ush *ush){
    
    char **way = NULL;
    bool flag_s = false;
    char *temp = NULL;
    bool flag_a = false;

    if (!ush->arguments || mx_list_size(ush->arguments) < 1) {
        return 1;
    }

    t_list *l_argv = ush->arguments;
    if(mx_strncmp(l_argv->data, "-*", 1) == 0){
        if(mx_strcmp(l_argv->data, "-s") == 0){
            flag_s = true;
        }
        else if(mx_strcmp(l_argv->data, "-a") == 0){
            flag_a = true;
        }
        else if(mx_strcmp(l_argv->data, "--") == 0){
            return 1;
        }
        else if(mx_strcmp(l_argv->data, "-sa") == 0|| mx_strcmp(l_argv->data, "-as") == 0){
            flag_a = true;
            flag_s = true;
        }
        else{
            mx_printerr("which: bad option: ");
            mx_printerr(l_argv->data);
            mx_printerr("\n");
            return 1;
        }
        l_argv = l_argv->next;
    }

    while(l_argv && ush->arguments){
        bool reserv = false;
        bool built_in = false;

        temp = l_argv->data;
        if(check_include(l_argv) == 0){
            reserv = true;
        }
        if(check_built_in(l_argv) == 0){
            built_in = true;
        }
        if(find_way(temp, way, flag_a, flag_s, reserv, built_in) == 1){
            return 1;
        }
        l_argv = l_argv->next;
    }

    return 0;
}
