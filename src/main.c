#include "../inc/ush.h"

int main() {
    t_our_ush *ush = NULL;
    ush_init(&ush);
    char* line = NULL;

    while (1) {
        if (isatty(0))
            mx_printstr("u$h> ");
        finder();

        isatty(0) ? (line = read_line(ush)) : (line = ush_read_line(ush));
        ush->input = line;
        if (line[0] == '\0') {
            mx_strdel(&line);
            continue;
        }

        ush_sep(line, ush);
        while(ush->cmd_queue) {
            int check = ush_parse(ush->cmd_queue->data, ush);
            if (check == -1) {
                continue;
            }

            ush_exec(ush);

            if (ush->num_cmd == 8) {
                mx_exit(ush);
            }

            mx_clear_ldata(&ush -> arguments);
            mx_clear_list(&ush -> arguments);
            ush -> arguments = NULL;
            mx_del_strarr(&ush->arguments_arr);
            ush->arguments_arr = NULL;

            mx_strdel((char **)&ush->cmd_queue->data);
            mx_pop_front(&ush->cmd_queue);
        }
        mx_strdel(&line);
        line = NULL;
    }

    return 0;
}
