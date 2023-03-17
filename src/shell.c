#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "headers/cmd.h"
#include "headers/shell.h"
#include "headers/utils.h"

static inline bool is_exit(const char *cmd) { return strcmp(cmd, "exit") == 0; }
static inline bool is_cd(const char *cmd) { return strcmp(cmd, "cd") == 0; }
static inline void exec_cmd(char *const *cmd_arr) {
    is_exit(cmd_arr[0]) ? exit(0) : NULL;
    if (is_cd(cmd_arr[0])) {
        chdir(cmd_arr[1]);
    }
    fork() != 0 ? wait(NULL) : execvp(cmd_arr[0], cmd_arr);
}

static int get_env(char **cmd_arr, int cmd_len) {
    for (int i = 0; i < cmd_len; ++i) {
        if (cmd_arr[i][0] == '$') {
            cmd_arr[i] = getenv(cmd_arr[i] + 1);
            if (cmd_arr == NULL) {
                fprintf(stderr, "No such environment variable: %s", cmd_arr[i]);
                return -1;
            }
        }
    }
    return 0;
}

static void read_cmd(Cmd *cmd) {
    printf("$ ");

    char c;
    int cur_num = 0, cur_len = 0;
    int max_cmd_num = CMD_NUM, max_cmd_len = CMD_LEN;

    while ((c = getchar()) != '\n') {
        if (cur_len == max_cmd_len)
            max_cmd_len = resize_cmd_len(cur_num, cur_len, cmd, max_cmd_len);
        else if (cur_num + 1 == max_cmd_num) {
            max_cmd_num = resize_cmd_set(cur_num, cmd, max_cmd_num);
        }

        if (is_redir_sign(c)) {
            cmd->redirect[cur_num] = c;
            ++cur_num;
            cur_len = 0;
        } else {
            cmd->sets[cur_num][cur_len] = c;
            ++cur_len;
        }
    }

    // trim unwanted prefix and suffix
    cmd->total = strlen(cmd->redirect) + 1;
    for (int k = 0; k < cmd->total; ++k) {
        cmd->sets[k] = trim(cmd->sets[k]);
    }
}

static void exec_uni_cmd(const Cmd *cmd) {
    // split the command into an array
    char *cmd_str = cmd->sets[0];
    int cmd_frag_num = calc_cmd_frag(cmd_str);
    char *cmd_arr[cmd_frag_num + 1];
    split_cmd(cmd_str, cmd_arr);

    // parse environment variables
    if (get_env(cmd_arr, cmd_frag_num) == -1)
        return;

    // execute the command
    exec_cmd(cmd_arr);
}

static void exec_multi_cmd(const Cmd *cmd) {
    /*int fd[2];*/
    /*pipe(fd);*/
    /*pid_t pid = fork();*/
    /*if (pid == 0) {*/
    /*dup2(fd[1], STDOUT_FILENO);*/
    /*close(fd[0]);*/
    /*close(fd[1]);*/
    /*execvp(cmd->sets[0], cmd->sets);*/
    /*} else {*/
    /*dup2(fd[0], STDIN_FILENO);*/
    /*close(fd[0]);*/
    /*close(fd[1]);*/
    /*execvp(cmd->sets[1], cmd->sets + 1);*/
    /*}*/
}

void init_shell(Shell **self) {
    if (!(*self = (Shell *)malloc(sizeof(Shell)))) {
        fprintf(stderr, "Failed to allocate memory in %s", __FUNCTION__);
        return;
    }
    (*self)->read_cmd = read_cmd;
    (*self)->exec_uni_cmd = exec_uni_cmd;
    (*self)->exec_multi_cmd = exec_multi_cmd;
}
