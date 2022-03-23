#include "../inc/ush.h"

int check_ush_input(char const *s) {
    int count_open_br = 0;
    int count_close_br = 0;
    int count1 = 0;
    int count2 = 0;
    int len = mx_strlen(s);
    for (int i = 0; i < len; i++) {
        if (s[i] == '{' && s[i-1] != '\\') {
            count_open_br++;
        }
        if (s[i] == '}' && s[i-1] != '\\') {
            count_close_br++;
        }
        if (s[i] == '\'') {
            if (s[i-1] != '\\' && i != len - 1)
                count1++;
            else if (i == len - 1) {
                count1++;
            }
        }
        if (s[i] == '\"') {
            if (s[i-1] != '\\' && i != len - 1)
                count2++;
            else if (i == len - 1) {
                count2++;
            }
        }
    }
    if (count_open_br != count_close_br) {
        mx_printerr("ush: syntax error: missing brace character\n");
        return -1;
    }
    if (count1 % 2 == 1) {
        mx_printerr("ush: syntax error: missing subtitution character\n");
        return -1;
    }
    if (count2 % 2 == 1) {
        mx_printerr("ush: syntax error: missing terminating character\n");
        return -1;
    }
    return 0;
}

static void backscape(int *position, char *line) {
    if (*position > 0) {
        for (int i = *position - 1; i < mx_strlen(line); i++) {
            line[i] = line[i + 1];
        }
        (*position)--;
    }
}

void cmd_edit(int keycode, int *position, char **line) {
    if (keycode == BACKSCAPE)
        backscape(position, *line);
}
