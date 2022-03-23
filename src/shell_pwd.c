#include "../inc/ush.h"

void mx_pwd(t_our_ush *ush) {
    t_list *check_opt = ush->arguments;
    char **arguments = NULL;
    arr_from_list(check_opt, &arguments);

    bool P_flag = false;
    bool L_flag = false;

    int i = mx_list_size(check_opt) - 1;
    while(i>=0)
    {
        int j = mx_strlen(arguments[i]) - 1;
        while(j > 0)
        {
            if (arguments[i][0] == '-' && arguments[i][j] != 'L' && arguments[i][j] != 'P') {
                mx_printerr("u$h: pwd: -");
                mx_printerr(&arguments[i][j]);
                mx_printerr(": invalid option\n");
                mx_printerr("pwd: usage: pwd [-LP]\n");
                ush->error = 1;
                return;
            }
            if (arguments[i][j] == 'L' && !P_flag) {
                L_flag = true;
            }
            if (arguments[i][j] == 'P' && !L_flag) {
                P_flag = true;
            }
            j--;
        }
        mx_strdel(&arguments[i]);
        arguments[i] = NULL;
        i--;
    }
    if (!L_flag && !P_flag) {
        P_flag = true;
    }
    free(arguments);
    arguments = NULL;
    if (!P_flag) {
        mx_printstr(ush->pwd);
        mx_printstr("\n");
    }
    else {
        char *directory = getcwd(NULL, 1024);
        if (directory) {
            mx_printstr(directory);
            mx_printstr("\n");
        }
        mx_strdel(&directory);
    }
    ush->error = 0;
}
