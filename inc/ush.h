#ifndef USH_H
#define USH_H

#include "../libmx/inc/libmx.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>

//here delete all MX_
#define MX_PATH (getenv("PATH"))
#define INPUT_SIZE  1024
#define CTRL_D      4
#define CTRL_C      3
#define TRL_Z      26
#define BACKSCAPE   127
#define ENTER       10


typedef struct s_process {

    int     id;
    pid_t   pid;
    pid_t   pgid;
    int     stat; //status
    int     foreground; //foregr
}t_process;

typedef struct s_our_ush {
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    sigset_t mask;

    char *pwd;
    char *prev_pwd;
    char *input;

    t_process *frontend;//t_proc *foreground;
    t_list *backend; //t_list *background;
    int count_backgrounds; //count_backgr
    char **arguments_arr; //argv_arr
    int num_cmd; //cmd_num
    bool qte; //quote
    t_list *arguments; //t_list *argv;
    int error; //err
    int line_size; //line_len
    int jobs_count; //count_job
    struct termios tmodes;
    t_list *cmd_queue;
    int make_redirection; //redirect
}               t_our_ush;


void ush_init(t_our_ush **ush); //void init(t_ush **ush);
char *ush_input();//char *input();
int ush_parse(char *src, t_our_ush *ush); //int ush_parser(char *src, t_ush *ush);
int check_ush_input(char const *s); //int check_input(char const *s);
void ush_exec(t_our_ush *ush); //void ush_execute(t_ush *ush); 
void ush_sep(char *s, t_our_ush *ush); //void ush_separator(char *s, t_ush *ush);
char **ush_split(char *s, t_our_ush *ush); //char **ush_strsplit(char *s, t_ush *ush);
char *delete_qte(char *src);//char *del_quot(char *src);
char *get_keys(t_our_ush *ush);
int backgr_new_id(t_list *background);//int bg_new_id(t_list *background);

char *read_line(t_our_ush *ush); //char *ush_reader(t_ush *ush);
char *ush_read_line(t_our_ush *ush); //char *ush_line_reader(t_ush *ush);
void signal_catch(int keycode, char **line, int *position, t_our_ush *ush);//void signal_detecter(int keycode, char **line, int *position, t_ush *ush);
void cmd_edit(int keycode, int *position, char **line); //void edit_cmd(int keycode, int *position, char **line);


int launch_proc(char **argv,t_our_ush *ush);//int launch_proccess(char **argv,t_ush *ush);
char *get_prog_path(char *name, const char *PATH); //char *get_programm_path(char *name, const char *PATH);
void finder(); //void detecter();
void free_ush(t_our_ush **ush); //void free_sh(t_ush **ush);

void mx_cd(t_our_ush *ush); //mx instead ush
void mx_pwd(t_our_ush *ush);
void mx_env(t_our_ush *ush);
int mx_which(t_our_ush *ush);
void mx_echo(t_our_ush *ush);
void mx_fg(t_our_ush *ush);
void mx_export(t_our_ush *ush);
void mx_unset(t_our_ush *ush);
void mx_exit(t_our_ush *ush);

void canon_enable();//void mx_enable_canon();
void canon_disable();//void mx_enable_canon();
struct termios *get_tty();//struct termios *mx_get_tty();

void arr_from_list(t_list *list, char ***arr); //void list_to_arr(t_list *list, char ***arr);

t_process *start_process(pid_t pid); //t_process *new_process(pid_t pid);
void add_backgr(t_process *proc, t_our_ush *ush); //void add_bg(t_process *proc, t_our_ush *ush);

#endif
