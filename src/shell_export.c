#include "../inc/ush.h"

static bool shell_isspace_orothers(char c) {
    if (mx_isspace(c)
    || (!mx_isalpha(c) 
    && !mx_isdigit(c) 
    && c != '_')) {
        return true;
    }
    return false;
}

static bool argcheck_exp (t_list *args) {
    if (args == NULL) {
    // if ^ doesnt work, use v
    // if (!shell_struct->argumments) {
        extern char **environ;
        for (int i = 0; environ[i]; i++) {
            printf("%s\n", environ[i]);
        }
        return false;
    }
    else {
        return true;
    }
}

static void split_eq(char *data, int *exit_error) {
    int index = -1;
    char **name = (char **)malloc(16);

    int counter = 0;
    while (data[counter]) {
        if(data[counter] == '=') {
            index = counter;
            break;
        }
        counter++;
    }

    if (index == 0) {
        char *err = mx_strndup(data + 1, mx_strlen(data) - 1);
        mx_printerr("ush: ");
        mx_printerr(err);
        mx_printerr(" not found\n");
        *exit_error = 1;
        return;
    }

    *name = mx_strndup(data, index);

    char *nametemp = *name;
    while (*nametemp) {
        if (shell_isspace_orothers(*nametemp)) { 
            mx_printerr("export: not valid in this context: ");
            mx_printerr(*name);
            mx_printerr("\n");
            *exit_error = 1;
            return;
        }
        nametemp++;
    }

    name[1] = mx_strndup(data + index + 1, mx_strlen(data) - index); 
    
    setenv(*name, name[1], 1);
}

static void export (char* data, int *exit_error) {
    int ind = -1;

    // del_quot?
    char *del_qdata = delete_qte(data);
    
    int counter = 0;
    while (del_qdata[counter]) {
        if (del_qdata[counter] == '=' && ind == -1) {
            ind = counter;
        }
        counter ++;
    }

    if (ind == -1) {
        char *del_qtemp = del_qdata;
        while (*del_qtemp) {
            if (shell_isspace_orothers(*del_qtemp)) {
                mx_printerr("export: not an identifier: ");
                mx_printerr(del_qdata);
                mx_printerr("\n");
                *exit_error = 1;
                return;
            }
            del_qtemp++;
        }

        setenv(del_qdata, "\'\'", 1);
        return;
    }
    else if (ind > -1) {
        split_eq(del_qdata, exit_error);
        return;
    }
}

void mx_export(t_our_ush *shell_s) {
    if (!argcheck_exp(shell_s->arguments)) {
        return;
    }

    t_list *temp_arglist = shell_s->arguments;
    int exit_error = 0;
    for (int i = 0; i < mx_list_size(shell_s->arguments); i++) {
        if (exit_error != 0) {
            break;
        }

        export(temp_arglist->data, &exit_error);
        temp_arglist = temp_arglist->next;
    }
    if (exit_error == 1) {
        shell_s->error = 1;
    }
    else {
        shell_s->error = 0;
    }
}
