#include "../inc/ush.h"

struct termios *get_tty(void) {
    static struct termios tty;

    return &tty;
}

static void handlerC(int signum) {
    (void)signum;
}

void finder() {
    signal(SIGINT, handlerC);
}


static void reverse_backscape(int *position, char *line)
{
    for (int i = *position; i < mx_strlen(line); i++)
    {
        line[i] = line[i + 1];
    }
}

void signal_catch(int keycode, char **line, int *position, t_our_ush *ush) {
    if (keycode == CTRL_C) {
        if (ush->frontend) {
            kill(ush->frontend->pid, SIGTERM);
        }
        for (int i = 0; i < mx_strlen(*line); i++) {
            *line[i] = '\0';
        }
    }
    if (keycode == CTRL_D) {
        if (mx_strcmp(*line, "") == 0) {
            printf("\n");
            //exit_ush(ush);
            exit(ush->error);
        }
        else {
            reverse_backscape(position, *line);
        }
    }
}

