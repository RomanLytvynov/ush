#include "../inc/ush.h"

void ush_exec(t_our_ush *ush) {
    if (ush->num_cmd == 8) {
            return;
    }

    switch (ush->num_cmd) {
        case 0:
                mx_export(ush);
                break;
        case 1:
                mx_unset(ush);
                break;
        case 2:
                mx_fg(ush);
                break;
        case 3:
                mx_env(ush);
                break;
        case 4:
                mx_cd(ush);
                break;
        case 5:             //pwd
                mx_pwd(ush);
                break;
        case 6:
                ush -> error = mx_which(ush);
                break;
        case 7: //echo
                mx_echo(ush);
                break;
        case 8: //exit
                return;
        case -1:
                ush -> error = launch_proc(ush->arguments_arr, ush);
                break;
    }
    ush -> num_cmd = -2;
}

static struct termios term_off(void) {
    struct termios tumbler;
    struct termios tty;

    tcgetattr (0, &tty);
    tumbler = tty;
    tty.c_lflag &= ~(ICANON|ECHO|ISIG|BRKINT|ICRNL
        |INPCK|ISTRIP|IXON|OPOST|IEXTEN);
    tty.c_cflag |= (CS8);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tcsetattr (0, TCSAFLUSH, &tty);
    return tumbler;
}

static void term_on(struct termios tumbler) {
    printf("\n");
    tcsetattr (0, TCSAFLUSH, &tumbler);
}

char *ush_read_line(t_our_ush *ush) {
    char *line = mx_strnew(1);
    char *res = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) < 0) {
        if (!isatty(0)) {
            ush->error = 0;
            exit(0);
        }
    }
    if(line[0] != '\0'){
        res = mx_strdup(line);
        mx_strdel(&line);
    }
    return res;
}

char *read_line(t_our_ush *ush) {
    char *line;
    struct termios tumbler;
    int stream1 = dup(1);
    int tty = open("/dev/tty", O_WRONLY);

    dup2(tty, 1);
    tumbler = term_off();
    ush->line_size = 1024;
    line = get_keys(ush);
    term_on(tumbler);
    dup2(stream1, 1);
    close(stream1);
    close(tty);
    return line;
}

