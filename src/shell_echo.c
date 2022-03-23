#include "../inc/ush.h"

static void chr_process(char *tmp, int i, char *res, int key){
    if (tmp[i] == 'a'){
        res[key] = '\a';
    }
    else if(tmp[i] == 'b'){
        res[key] = '\b';
    }
    else if(tmp[i] == 'f'){
        res[key] = '\f';
    }
    else if(tmp[i] == 'n'){
        res[key] = '\n';
    }
    else if(tmp[i] == 'r'){
        res[key] = '\r';
    }
    else if(tmp[i] == 't'){
        res[key] = '\t';
    }
    else if(tmp[i] == 'v'){
        res[key] = '\v';
    }
}

char *echo_process_char(char *src, bool flag_E) 
{
int quo_count = 0;
    int double_counter_buff = 0;    
    int one_flag = 1;
    int two_flag = 1;
    

    char *tmp = mx_strdup(src);
    int count_quot = 0;
    for(int i = 0; i < mx_strlen(tmp); i++) {
        if (tmp[i] == '\'' ||tmp[i] == '\"') {
            count_quot++;
        }
    }
    
    int size = mx_strlen(tmp) - count_quot;
    char *res = mx_strnew(size);
    int key = 0;
/*char *process_incomands(char *arg, t_for_echo echo) {
    char *include_cmd[] = {"\\a","\\b","\\f","\\n","\\r","\\t","\\v", NULL};
    char *final[] = {"\a","\b","\f","\n","\r","\t","\v", NULL};
    char *res = mx_strnew(ARG_MAX);
    int index = 0;
    if ((index = mx_get_substr_index(arg, "\\c")) >= 0) {
        strncpy(res, arg, index);
        echo.n = true;
    }
    else {
        strcpy(res, arg);
    }
    char *tmp = process_slashes(res);
    for (int i = 0; include_cmd[i] != NULL; i++) {
        if (strstr(tmp, include_cmd[i])) {
            tmp = mx_replace_substr(tmp, include_cmd[i], final[i]);
        }
    }
    free(res);
    return tmp;
}*/
    for (int i = 0; i < mx_strlen(tmp); i++) {
        if(flag_E == false){
            if(tmp[i] == '\'' || tmp[i] == '\"'){
                if(tmp[i] == '\''){
                    one_flag *= -1;
                }
                else{
                    two_flag *= -1;
                }
            }
            else if(tmp[i] == '\\' && one_flag == -1){
                quo_count++;
                if(quo_count % 2 == 1){
                    i++;
                    if(tmp[i] == 'a'){
                        res[key] = '\a';
                    }
                    else if(tmp[i] == 'b'){
                        res[key] = '\b';
                    }
                    else if(tmp[i] == 'f'){
                        res[key] = '\f';
                    }
                    else if(tmp[i] == 'n'){
                        res[key] = '\n';
                    }
                    else if(tmp[i] == 'r'){
                        res[key] = '\r';
                    }
                    else if(tmp[i] == 't'){
                        res[key] = '\t';
                    }
                    else if(tmp[i] == 'v'){
                        res[key] = '\v';
                    }
                    else{
                        res[key] = tmp[i];
                        i--;
                    }
                    key++;
                }
            }
            else if(tmp[i] == '\\' && two_flag == -1){
                if(tmp[i + 1] != '\\'){
                    i++;
                    chr_process(tmp, i, res, key);
                    key++;
                }
                else if(tmp[i + 2] != '\\'){
                    i += 2;
                    chr_process(tmp, i, res, key);
                    key++;
                }
                else if(tmp[i + 3] != '\\'){
                    i += 3;
                    res[key] = '\\';
                    key++;
                    res[key] = tmp[i];
                    key++;
                }
                else if(tmp[i + 4] != '\\'){
                    i += 4;
                    res[key] = '\\';
                    key++;
                    res[key] = tmp[i];
                    key++;
                }
                else if(tmp[i + 5] != '\\'){
                    i += 5;
                    res[key] = '\\';
                    key++;
                    chr_process(tmp, i, res, key);
                    key++;
                }
                else if(tmp[i + 6] != '\\'){
                    i += 6;
                    res[key] = '\\';
                    key++;
                    chr_process(tmp, i, res, key);
                    key++;
                }
            }
            else if(tmp[i] == '\\' && one_flag == 1 && two_flag == 1){
                if(tmp[i + 1] != '\\'){
                    i++;
                    res[key] = tmp[i];
                    key++;
                }
                else if(tmp[i + 2] != '\\'){
                    i += 2;
                    chr_process(tmp, i, res, key);
                    key++;
                }
                else if(tmp[i + 3] != '\\'){
                    i += 3;
                    chr_process(tmp, i, res, key);
                    key++;
                }
                else if(tmp[i + 4] != '\\'){
                    i += 4;
                    res[key] = '\\';
                    key++;
                    res[key] = tmp[i];
                    key++;
                }
                else if(tmp[i + 5] != '\\'){
                    i += 5;
                    res[key] = '\\';
                    key++;
                    res[key] = tmp[i];
                    key++;
                }
                else if(tmp[i + 6] != '\\'){
                    i += 6;
                    res[key] = '\\';
                    key++;
                    chr_process(tmp, i, res, key);
                    key++;
                }
            }
            else{
                res[key] = tmp[i];
                key++;
            }
        }
        else{
            if((tmp[i] == '\'' || tmp[i] == '\"') && flag_E == true){
                if(tmp[i] == '\''){
                    one_flag *= -1;
                }
                else{
                    two_flag *= -1;
                }
            }
            else{
                if(one_flag == -1){
                    res[key] = tmp[i];
                    key++;
                }
                else if(two_flag == -1 && tmp[i] == '\\'){
                    double_counter_buff++;
                    if(tmp[i + 1] != '\\'){
                        if(double_counter_buff == 1 || double_counter_buff == 2){
                            res[key] = tmp[i];
                            key++;
                        }
                        else if(double_counter_buff == 3 || double_counter_buff == 4){
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                        }
                        else if(double_counter_buff == 5 || double_counter_buff == 6){
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                        }
                        else{
                            res[key] = tmp[i];
                            key++;
                        }
                    }
                }
                else if(one_flag == 1 && two_flag == 1 && tmp[i] == '\\'){
                    double_counter_buff++;
                    if(tmp[i + 1] != '\\'){
                        if(double_counter_buff == 1){
                            continue;
                        }
                        else if(double_counter_buff == 3 || double_counter_buff == 2){
                            res[key] = tmp[i];
                            key++;
                        }
                        else if(double_counter_buff == 4 || double_counter_buff == 5){
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                        }
                        else if(double_counter_buff == 6){
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                            res[key] = tmp[i];
                            key++;
                        }
                        else{
                            res[key] = tmp[i];
                            key++;
                        }
                    }
                }
                else{
                    res[key] = tmp[i];
                    key++;
                }
            }
        }
    }
    res[key] = '\0';
    mx_strdel(&tmp);
    return res;
}

void mx_echo(t_our_ush *ush){

    bool flag_e = false;
    bool flag_E = false;
    bool flag_n = false;


    if (!ush->arguments) {
        mx_printchar('\n');
        return;
    }

    char **arr = NULL;
    t_list *f_list = NULL;

    if(mx_strncmp(ush->arguments->data, "-*", 1) == 0)
    {
        if(mx_strcmp(ush->arguments->data, "-E") == 0){
            flag_E = true;
        }
        else if(mx_strcmp(ush->arguments->data, "-n") == 0){
            flag_n = true;
        }
        else if(mx_strcmp(ush->arguments->data, "-e") == 0){
            flag_e = true;
        }
        else if(mx_strcmp(ush->arguments->data, "-En") == 0|| mx_strcmp(ush->arguments->data, "-nE") == 0){
            flag_E = true;
            flag_n = true;
        }
        ush->arguments = ush->arguments->next;
    }

    arr_from_list(ush->arguments, &arr);
/*char *process_slashes(char *str) {
    char *res = (char *)malloc(mx_strlen(str) + 1);
    int len = 0;

    for (int i = 0; i <= mx_strlen(str); i++) {
        if (str[i] == '\\' && str[i + 1] == '\\') {
            i++;
        }
        if (str[i] == '\\' && str[i + 1] == 'e') {
            if (str[i + 2] != '\\') {
                (i) += 3;
            }
            else {
                (i) += 2;
            }
        }
        res[len] = str[i];
        len++;
    }
    res[len] = '\0';
    return res;
}*/
    for(int i = 0; i < mx_list_size(ush->arguments); i++){
        if(arr[i]){
            mx_push_back(&f_list, echo_process_char(arr[i], flag_E));
        }
        mx_strdel(&arr[i]);
    }
    free(arr);
    arr = NULL;

        if(!f_list)
    {
        return;
    }
    t_list *output = f_list;
    while(output){
        mx_printstr(output->data);
        if(output->next != NULL){
            mx_printchar(' ');
        }
        output = output->next;
    }
    if(!flag_n){
        mx_printchar('\n');
    }

    mx_clear_ldata(&f_list);
    mx_clear_list(&f_list);

    ush->error = 0;
}
