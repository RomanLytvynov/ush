#include "../inc/ush.h"

void fger(char *str, t_our_ush *ush) {
    if (str[0] != '%') {
        mx_printerr("fg: job not found: ");
        mx_printerr(str);
        mx_printerr("\n");
        ush->error = 1;
        return;
    }
    int personal_id = atoi(str + 1);

    t_list *tmp = ush->backend;
    for (int i = 0; i < mx_list_size(ush->backend); i++) {
        if (personal_id == ((t_process *)(tmp->data))->id) {
            pid_t pid_chil = ((t_process *)(tmp->data))->pid;
            ush->frontend = ((t_process *)(ush->backend->data));
            
            canon_disable();
            tcsetpgrp(STDIN_FILENO, pid_chil);
            kill(pid_chil, SIGCONT);
            int stat = 0;
            waitpid(pid_chil, &stat, WUNTRACED);
            if (!WIFSTOPPED(stat)) {
                free(ush->backend->data);
                mx_pop_back(&ush->backend);
                ush->count_backgrounds--;
            }

            tcsetpgrp(STDIN_FILENO, getpgrp());
            canon_enable();
            ush->error = 0;
        }
        tmp = tmp->next;
    }
    mx_printerr("fg: ");
    mx_printerr(str);
    mx_printerr(": no such job\n");
    ush->error = 1;
}

void mx_fg(t_our_ush *ush) {
    
    
    if (!ush->arguments) {
        if (!ush->count_backgrounds) {
            mx_printerr("fg: no current job\n");
            ush->error = 1;
            return;
        }
        t_list *l_apperance = ush->backend;
        while (l_apperance->next) {
            l_apperance = l_apperance->next;
        }
        pid_t pid_chil = ((t_process *)(l_apperance->data))->pid;
        ush->frontend = ((t_process *)(ush->backend->data));
        /*void mx_env(char **term_arg_cmd) {
    ///error status
    setenv("?", "0", 1);
    int env_size = 0;
    int size = mx_arrsize(term_arg_cmd);
    //////////
    int i = 1;
    char **env = 0;
    char **p_env = environ;
    char *path = 0;
    
    ///////
    while (*p_env) {
        env = push_back_str(env, &env_size, *p_env);
        p_env++;
    }*/
        canon_disable();
        tcsetpgrp(STDIN_FILENO, pid_chil);
        kill(pid_chil, SIGCONT);
        int stat = 0;
        waitpid(pid_chil, &stat, WUNTRACED);
        if (!WIFSTOPPED(stat)) {
            free(ush->backend->data);
            mx_pop_back(&ush->backend);
            ush->count_backgrounds--;
        }

        tcsetpgrp(STDIN_FILENO, getpgrp());
        canon_enable();
        ush->error = 0;
        return;
    }

    t_list *tmp = ush->arguments;

    for (int i = 0; i < mx_list_size(ush->arguments); i++) {
        fger(tmp->data, ush);
        tmp = tmp->next;
    }
    return;
}
