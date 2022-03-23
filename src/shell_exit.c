#include "../inc/ush.h"

void mx_exit(t_our_ush *shell_s) {
    if (shell_s->count_backgrounds) {
        mx_printerr("ush: you have suspended jobs.\n");
    }
    else {
        int index = 0;
        if (shell_s->arguments) {
            tcsetattr(STDIN_FILENO, TCSADRAIN, get_tty());
            index = mx_atoi(shell_s->arguments->data);
            free_ush(&shell_s);
            exit(index);
        }
        else {
            tcsetattr(STDIN_FILENO, TCSADRAIN, get_tty());
            index = shell_s->error;
            free_ush(&shell_s);
            exit(index);
        }
    }
}
