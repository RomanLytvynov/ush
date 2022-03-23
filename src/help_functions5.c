#include "../inc/ush.h"

#include "../inc/ush.h"

int ush_parse(char *src, t_our_ush *ush) {
    char *cmd_table[] = {"export", "unset", "fg","env", 
                         "cd", "pwd", "which", "echo", 
                         "exit"};

    char *tr_src = mx_strtrim(src);
    //printf("%s", tr_src);
    //printf("\n");
    char **src_arr = ush_split(tr_src, ush);
    ush->arguments_arr = ush_split(tr_src, ush);
    mx_strdel(&tr_src);
    if (!src_arr) {
        return 1;
    }
    bool check = false;
    for (int i = 0; i < 9; i++) {
        if (mx_strcmp(cmd_table[i], src_arr[0]) == 0) {
            check = true;
            ush->num_cmd = i;
        }
    }
    if (!check) {
        ush->num_cmd = -1;
        //return 127;
    }
    int count = 0;
    for (int i = 0; src_arr[i] != NULL; i++) {
        count++;
    }
    for (int i = 1; src_arr[i] != NULL; i++) {
        bool flag = false;
        int flag_d = 1;
        bool dnt = false;

        for (int j = 0; j < mx_strlen(src_arr[i]); j++) {
            if (src_arr[i][j] == '$') {
                if (ush ->input) {
                    bool esc = false;
                    for (int k = 1; k < mx_strlen(ush->input); k++) {
                        if (ush->input[k - 1] == '\\' && ush->input[k] == '$') {
                            esc = true;
                        }
                    }
                    if (esc)
                        continue;
                }
                bool skobka = src_arr[i][j + 1] == '(' ? true : false;
                //fprintf(stderr, "src %s\n", src_arr[i]);
                int count_sk = 0;
                int k = j + 1;
                //int start = k;
                int len = 0;
                if(src_arr[i][j + 1] == '?' && ush -> num_cmd == 7){
                    //mx_printint(ush->err);
                    //mx_printchar('\n');
                    char *return_str = mx_itoa(ush->error);
                    char *temp1 = mx_replace_substr(src_arr[i], "$?", return_str);
                    mx_strdel(&src_arr[i]);
                    src_arr[i] = temp1;
                    mx_strdel(&return_str);
                    break;
                }

                if(src_arr[i][k] == '\''){
                    flag_d *= -1;
                }
 
                if (!skobka){
                    while (/*src_arr[i][k] != ' ' &&*/ src_arr[i][k] != '\'' && src_arr[i][k] != '\"' && src_arr[i][k] != '\0') {
                        len++;
                        k++;
                        if(src_arr[i][k] == '\''){
                            flag_d *= -1;
                        }
                        if (src_arr[i][k] == '}') {
                            len++;
                            break;
                        }
                    }
                }
                else {
                    while (src_arr[i][k] != '\0') {
                        if (src_arr[i][k] == '(') {
                            count_sk++;
                        }
                        if (src_arr[i][k] == ')') {
                            count_sk--;
                        }
                        k++;
                        len++;
                        if (count_sk == 0) {
                            break;
                        }
                        //fprintf(stderr, "%d %c",  count_sk, src_arr[i][k]);
                    }
                    if (src_arr[i][k] == ')' && src_arr[i][k - 1] == ')') {
                        len--;
                    }
                }

                if(flag_d == 1){
                    /*while (src_arr[i][j + 1 + len] != ')') {
                        len--;
                    }*/
                    mx_del_extra_spaces(*src_arr);
                    char *env_var = mx_strndup(src_arr[i] + j + 1, len);
                    //fprintf(stderr, "env var %s\n", env_var);
                    char *temp = mx_strjoin("$", env_var);

                    if(env_var[0] == '{' /*&& env_var[mx_strlen(env_var) - 1] == '}'*/){
                        char *temp_temp = mx_strdup(env_var);
                        j += len;
                        int r_count = 1;
                        while (env_var[mx_strlen(env_var) - r_count] != '}') {
                            r_count++;
                        }
                        int size = mx_strlen(temp_temp) - r_count;
                        char *res_s = mx_strnew(size);
                        int k = 0;

                        for(int i = 1; i < mx_strlen(temp_temp) - r_count; i++){
                            if (temp_temp[i] != '}')
                            {
                                res_s[k] = temp_temp[i];
                                k++;
                            }
                        }
                        //printf("%s", res_s);
                        char *res = getenv(res_s);

                        mx_strdel(&res_s);
                        mx_strdel(&env_var);
                        if (res) {
                            //printf("%s\n", mx_replace_substr(src_arr[i], temp, res));
                            //char *temp_src = src_arr[i];
                            src_arr[i] = mx_replace_substr(src_arr[i], temp, res);
                            //mx_strdel(&temp_src);
                            bool is_br = false;
                            for (int q = 0; q < mx_strlen(src_arr[i]) - 1; q++) {
                                if (src_arr[i][q] == '$' && src_arr[i][q + 1] == '{') {
                                    is_br = true;
                                }
                            }
                            if (!is_br)
                                mx_push_back(&(ush->arguments), mx_strdup(src_arr[i]));
                            flag = true;
                        }
                        else{
                            dnt = true;
                        }
                    }
                    else if(env_var[0] == '(' /*&& env_var[mx_strlen(env_var) - 1] == ')'*/){
                        if (ush->num_cmd == 7 && src_arr[i][0] == '"' && count == 1) {
                            src_arr = ush_split(mx_strndup(src_arr[i] + 1, mx_strlen(src_arr[i]) - 2), ush);
                            //fprintf(stderr, "%s\n", mx_strndup(src_arr[i] + 1, mx_strlen(src_arr[i]) - 2));
                            i--;
                        }
                        char *temp_temp = mx_strdup(env_var);
                        j += len;
                        int r_count = 1;
                        while (env_var[mx_strlen(env_var) - r_count] != ')') {
                            r_count++;
                        }
                        r_count++;
                        int size = mx_strlen(temp_temp) - r_count;
                        char *res_s = mx_strnew(size);
                        int k = 0;

                        for(int i = 1; i < mx_strlen(temp_temp) - r_count; i++){
                            /*if(temp_temp[i] == '(' || temp_temp[i] == ')'){
                                continue;
                            }*/
                            //else
                            if (temp_temp[i] != ')')
                            {
                                res_s[k] = temp_temp[i];
                                k++;
                            }
                        }
                        //printf("temp_temp %s\n", temp_temp);
                        //printf("res %s\n", res_s);

                        char *file = ".redir";


                        /*fflush(stdout);
                        int fd1 = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        int fd2 = dup(STDOUT_FILENO);
                        dup2(fd1, STDOUT_FILENO);
                        close(fd1);*/
                        t_our_ush *temp_ush;
                        ush_init(&temp_ush);
                        temp_ush->make_redirection = 1;
                        //fpos_t pos;
                        int fd;
                        //if (!ush->redirect) {
                        //fgetpos(stdout, &pos);  // save the position in the file stream
                        fd = dup(fileno(stdout));  // use the dup() function to create a copy of stdout
        
                        freopen(file, "w", stdout);  // redirect stdout
                        //fclose(stdout);
                        //}
                        FILE *file_p = fopen(file, "w");
                        fclose(file_p);
                        int cmd_num = ush->num_cmd;
                        //ush->redirect++;
                        temp_ush->num_cmd = ush->num_cmd;
                        char *var = mx_strndup(env_var + 1, mx_strlen(env_var) - r_count);
                        ush_parse(var, temp_ush);
                        mx_strdel(&var);
                        ush_exec(temp_ush);
                        free_ush(&temp_ush);
                        ush->num_cmd = cmd_num;
                        //ush->redirect--;
                        //if (!ush->redirect) {
                        fflush(stdout);
 
                        dup2(fd, fileno(stdout));  // restore the stdout

                        close(fd);
                        clearerr(stdout);  

                        //fsetpos(stdout, &pos); // move to the correct position
                        //}
                        //fopen(stdout);
                        //fflush(stdout);
                        //dup2(fd2, STDOUT_FILENO);
                            //return err_report("Failed to reinstate standard output\n");
                        //close(fd2);
                        //mx_strdel(&res_s);
                        
                        mx_strdel(&env_var);
                        char *repl = mx_file_to_str(file);
                        if (!ush->make_redirection)
                            remove(file);
                        char *repl_tr = mx_del_extra_spaces(repl);
                        mx_strdel(&repl);
                        //fprintf(stderr, "3\n");
                        if (repl_tr) {
                            src_arr[i] = mx_replace_substr(src_arr[i], temp, repl_tr);
                            //mx_strdel(&temp_src);
                            bool is_br = false;
                            for (int q = 0; q < mx_strlen(src_arr[i]) - 1; q++) {
                                if (src_arr[i][q] == '$' && src_arr[i][q + 1] == '(') {
                                    is_br = true;
                                }
                            }
                            if (!is_br)
                                mx_push_back(&(ush->arguments), mx_strdup(src_arr[i]));
                            
                            //mx_push_back(&(ush->argv), mx_replace_substr(src_arr[i], temp, repl_tr)); 
                            //fprintf(stderr, "\n%d   %s | %s | %s\n", ush->redirect, src_arr[i],  temp, mx_replace_substr(src_arr[i], temp, repl_tr));
                            flag = true;
                        }
                        else {
                            dnt = true;
                        }
                        mx_strdel(&repl_tr);
                        mx_strdel(&temp);
                        mx_strdel(&temp_temp);
                        mx_strdel(&res_s);
                    }
                    else{
                        char *res = getenv(env_var);
                        mx_strdel(&env_var);

                        if (res) {
                            src_arr[i] = mx_replace_substr(src_arr[i], temp, res);
                            mx_push_back(&(ush->arguments), mx_strdup(src_arr[i]));
                            flag = true;
                        }
                        else{
                            dnt = true;
                        }
                    }
                }
            }     
        }
        if (!flag && !dnt)
            mx_push_back(&(ush->arguments), mx_strdup(src_arr[i]));
        }
    
    mx_strdel(&tr_src);
    mx_del_strarr(&src_arr);
    return 0;
}

int backgr_new_id(t_list *background) {
    if (!background) {
        return 1;
    }
    t_list *first = background;
    while (first->next) {
        if (((t_process *)(first->data))->id != ((t_process *)(first->next->data))->id - 1) {
            return ((t_process *)(first->data))->id + 1;
        }
        first = first->next;
    }
    return mx_list_size(background);
}

t_process *start_process(pid_t pid) {
    t_process *new = (t_process *)malloc(sizeof(t_process));
    new->pid = pid;
    new->pgid = getpgid(pid);
    new->stat = 1;
    new->foreground = 1;
    return new;
}

void add_backgr(t_process *proc, t_our_ush *ush) {
    tcsetpgrp(STDIN_FILENO, getpgrp());
    canon_enable();
    proc->id = backgr_new_id(ush->backend);
    mx_push_back(&ush->backend, proc);
    ush->count_backgrounds++;
    printf("[%d]    %d suspended  %s\n", ush->count_backgrounds, proc->pid, ush->arguments_arr[0]);
}

void ush_sep(char *s, t_our_ush *ush) {
    if (!s) {
        return;
    }
    if (check_ush_input(s)) {
        return;
    }
    int word_count = 0;
    bool is_mark = false;
    if (s[0] == '\'' || s[0] == '\"') {
        is_mark = true;
    }
    
    int i = 1;
    for (; i < mx_strlen(s); i++) {
        if ((s[i] == '\'' || s[i] == '\"') 
        && s[i - 1] != '\\') {
            if (!is_mark) {
                if (s[i - 1] == ' ')
                    is_mark = true;
            }
            else {
                is_mark = false;
                //word_count++;
            }
        }

        if (!is_mark && s[i - 1] != '\\'&& s[i] == ';' && s[i + 1] != ';' && s[i + 1] != '\'' && s[i + 1] != '\"' && s[i + 1] != '\0' && i < mx_strlen(s) - 1) {
            word_count++;
        }
    }
    if (s[0] != '\'' && s[0] != '\0' && s[0]!= '\"') {
        word_count++;
    }
    if (!word_count) {
        mx_push_back(&ush->cmd_queue, mx_strdup(s));
        return;
    }
    int start = 0;
    is_mark = false;
    char delim = ';';
    for (int i = 0; i < word_count; i++) {
        delim = ';';
        while (s[start] == delim) {
            start++;
        }
        if (start == mx_strlen(s) - 1) {
            break;
        }
        int start_word = start;
        int word_len = 0;
        
        char quote;
        if (s[start] == '\'' || s[start] == '\"') {
            is_mark = is_mark == true ? false : true;
            quote = s[start] == '\"' ? '\"' : '\'';
            word_len++;
        }

        while (s[start_word + word_len] != '\0') {
            if (!is_mark ) {
                if (s[start_word + word_len] == '\'' 
                || s[start_word + word_len] == '\"') {
                    is_mark = true;
                    quote = s[start_word + word_len] == '\"' ? '\"' : '\'';
                }
                if (s[start_word + word_len] == delim && s[start_word + word_len - 1] != '\\') {
                    break;
                }
            }
            else if (is_mark) {
                if (s[start_word + word_len] == quote) {
                    //word_len++;
                    is_mark = false;
                }
            }
            word_len++;
            if (s[start_word + word_len] == delim && !is_mark && s[start_word + word_len - 1] != '\\') {
                break;
            }
        }
        if(s[start_word] == '$'){
            ush->qte = true;
        }

        char *temp1 = mx_strnew(word_len);
        mx_strncpy(temp1, s + start, word_len);
        mx_push_back(&ush->cmd_queue, temp1);
        
        start += word_len;
    }
}

char **ush_split(char *str0, t_our_ush *ush) {
    if (!str0) {
        return NULL;
    }

    char *s = mx_replace_substr(str0, "@", " ");

    if (check_ush_input(s)) {
        return NULL;
    }
    int word_count = 0;
    bool is_mark = false;
    if (s[0] == '\'' || s[0] == '\"') {
        is_mark = true;
    }
    int i = 1;
    for (; i < mx_strlen(s); i++) {
        if (s[i] == '(') {
            if (s[i - 1] == ' ') {
                word_count++;
            }
            while (s[i] != ')') {
                i++;
            } 
        }
        if ((s[i] == '\'' || s[i] == '\"') ) {
            if (!is_mark) {
                if (s[i - 1] == ' ')
                    is_mark = true;
            }
            else {
                if (s[i-1] != '\\' && i != mx_strlen(s) - 1) {
                    is_mark = false;
                    word_count++;
                }
                else if (i == mx_strlen(s) - 1) {
                    is_mark = false;
                    word_count++;
                }
            }
        }

        if (i < mx_strlen(s) - 1) {
            if (!is_mark && s[i - 1] != '\\' && s[i] == ' ' && s[i + 1] != ' ' && s[i + 1] != '\'' && s[i + 1] != '\"') {
                word_count++;
            }
        }
    }
    if (s[0] != '\'' && s[0] != '\0' && s[0]!= '\"') {
        word_count++;
    }
    //printf("wc %d\n", word_count);
    char **words = (char **)malloc(8 * (word_count + 1));
    int start = 0;
    is_mark = false;
    char delim = ' ';
    for (int i = 0; i < word_count; i++) {
        delim = ' ';
        while (s[start] == delim) {
            start++;
        }

        int start_word = start;
        int word_len = 0;
        
        char quote;
        if (s[start] == '\'' || s[start] == '\"') {
            is_mark = is_mark == true ? false : true;
            quote = s[start] == '\"' ? '\"' : '\'';
            word_len++;
        }

        while (s[start_word + word_len] != '\0') {
            if (s[start + word_len] == '(' && s[start_word + word_len - 1] != '\\') {
                delim = ')';
            }
            if (!is_mark ) {
                if (s[start_word + word_len] == '\'' 
                || s[start_word + word_len] == '\"') {
                    is_mark = true;
                    quote = s[start_word + word_len] == '\"' ? '\"' : '\'';
                }
                if (s[start_word + word_len] == delim && s[start_word + word_len - 1] != '\\') {
                    while (delim == ')' && s[start_word + word_len] == delim && start_word + word_len < mx_strlen(s)) {
                        word_len++;
                        //continue;
                    }
                    if (delim == ')' && s[start_word + word_len] != delim && s[start_word + word_len] != ' ' && start_word + word_len < mx_strlen(s)) {
                        delim = ' ';
                        word_len++;
                        continue;
                    }
                    break;
                }
            }
            else if (is_mark) {
                if (s[start_word + word_len] == quote) {
                    //word_len++;
                    is_mark = false;
                }
            }
            word_len++;
            if (s[start_word + word_len] == delim && !is_mark && s[start_word + word_len - 1] != '\\') {
                if (delim == ')') {
                    //word_len++;
                    continue;
                }
                
                break;
            }
        }
        if(s[start_word] == '$'){
            ush->qte = true;
        }
        if (i == 1) {
            char *cmd_table[] = {"export", "unset", "fg","env", 
                         "cd", "pwd", "which", "echo", 
                         "exit"};
            bool check = false;
            for (int j = 0; j < 9; j++) {
                if (mx_strcmp(cmd_table[j], words[0]) == 0) {
                    check = true;
                    ush->num_cmd = j;
                }
            }
            if (!check) {
                ush->num_cmd = -1;
                //return 127;
            }
        }
        char *temp1 = mx_strnew(word_len);
        mx_strncpy(temp1, s + start, word_len);
        if (ush->num_cmd != 7) {
            char *q = delete_qte(temp1);
            mx_strdel(&temp1);
            char *temp2 = mx_replace_substr(q, "\\\"", "\"");
            mx_strdel(&q);
            temp1 = mx_replace_substr(temp2, "\\\'", "\'");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\ ", " ");
            mx_strdel(&temp1);
            temp1 = mx_replace_substr(temp2, "\\`", "`");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\$", "$");
            mx_strdel(&temp1);
            temp1 = mx_replace_substr(temp2, "\\(", "(");
            mx_strdel(&temp2);
            temp2 = mx_replace_substr(temp1, "\\)", ")");
            mx_strdel(&temp1);
            words[i] = mx_replace_substr(temp2, "\\\\", "\\");
            mx_strdel(&temp2);
            //printf("%s\n", words[i]);
            //words[i] = mx_replace_substr(temp1, "\\\\", "\\");
            //mx_strdel(&temp1);
        }
        else if(ush->num_cmd == -1){
            char *q = delete_qte(temp1);
            mx_strdel(&temp1);
            words[i] = q;
        }
        else {
            words[i] = temp1;
        }
        //printf("%s\n", words[i]);
        if (words[i][0] == '~'&& words[i][1] != '-'&& words[i][1] != '+') {
            char *repl = mx_strdup(getenv("HOME"));
            char *usr = NULL;
            if (words[i][1] != '/' && words[i][1] != '\0' 
            && words[i][1] != '-'&& words[i][1] != '+') {
                usr = mx_strndup((words[i] + 1), mx_strlen(words[i]) - 1);
            }
            int split_len = 1;
            if (usr) {
                while (repl[split_len] != '\0') {
                    if (repl[split_len] == '/') {
                        break;
                    }
                    split_len++;
                }
                char *temp = mx_strndup(repl, split_len + 1);
                mx_strdel(&repl);
                repl = mx_strjoin(temp, usr);
                mx_strdel(&temp);
            }
            char *path = NULL;
            if (!usr) {
                path = mx_replace_substr((words[i]),"~",repl);
                mx_strdel(&repl);
            }
            else {
                path = repl;
            }
            free(words[i]);
            mx_strdel(&usr);
            words[i] = path;
        }
        if (mx_strstr((words[i]), "~+")) {
            char *repl = getenv("PWD");
            char *path = mx_replace_substr((words[i]),"~+",repl);
            free(words[i]);
            words[i] = path;
        }
        if (mx_strstr((words[i]), "~-")) {
            char *repl = getenv("OLDPWD");
            char *path = mx_replace_substr((words[i]),"~-",repl);
            free(words[i]);
            words[i] = path;
        }

        start += word_len;
    }

    words[word_count] = NULL;

    return words;
}

void free_ush(t_our_ush **ush) {
    if (malloc_size((*ush) -> pwd) != 0)
        mx_strdel(&((*ush)->pwd));
    if (malloc_size((*ush) -> prev_pwd) != 0)
        mx_strdel(&((*ush)->prev_pwd));

    mx_clear_ldata(&(*ush)->arguments);
    mx_clear_list(&(*ush)->arguments);

    mx_clear_ldata(&(*ush)->cmd_queue);
    mx_clear_list(&(*ush)->cmd_queue);

    mx_del_strarr (&(*ush)->arguments_arr);

    mx_strdel(&(*ush)->input);

    free(*ush);
    *ush = NULL;
}
