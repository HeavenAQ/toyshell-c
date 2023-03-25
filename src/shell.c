#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "headers/cmd.h"
#include "headers/shell.h"
#include "headers/utils.h"
#define PATH_MAX 1 KB

static inline bool is_exit(const char *cmd) { return strcmp(cmd, "exit") == 0; }
static inline bool is_cd(const char *cmd) { return strcmp(cmd, "cd") == 0; }
static inline void exec_cmd(char *const *cmd_arr)
{
    is_exit(cmd_arr[0]) ? exit(0) : NULL;
    if (is_cd(cmd_arr[0])) {
        chdir(cmd_arr[1]);
    }
    execvp(cmd_arr[0], cmd_arr);
}

#if 0 
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
#endif

static char **prep_cmd(const Cmd *cmd, const int i)
{
    char *cmd_str = cmd->sets[i];
    char **cmd_arr = malloc(sizeof(char *) * CMD_NUM);

    // split the command into an array
    int cmd_frag_num = calc_cmd_frag(cmd_str);
    init_str_arr(cmd_arr, cmd_frag_num, CMD_LEN);
    split_cmd(cmd_str, cmd_arr);
    return cmd_arr;
}

static void read_cmd(Cmd *cmd)
{
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
    int redir_len = strlen(cmd->redirect);
    cmd->total = redir_len + 1;
    for (int k = 0; k < cmd->total; ++k) {
        cmd->sets[k] = trim(cmd->sets[k]);
    }

    // check if the command is a daemon
    if (cmd->sets[cmd->total][0] == '\0' &&
        cmd->redirect[redir_len - 1] == '&') {

        cmd->is_background = true;
        cmd->total -= 1;
        cmd->redirect[cmd->total - 1] = '\0';
    }
}

static void exec_uni_cmd(const Cmd *cmd)
{
    // split the command into an array
    char **cmd_arr = prep_cmd(cmd, 0);
    exec_cmd(cmd_arr);
}

static void pipeline(char *const *cmd1)
{
    int fd[2];
    pipe(fd);
    if (fork() != 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        exec_cmd(cmd1);
    } else {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
    }
}

static void redir(char **cmd_arr, const char *file, const int std_fd)
{
    const int permission =
        std_fd == STDOUT_FILENO ? (O_WRONLY | O_CREAT | O_TRUNC) : O_RDONLY;

    int fd = open(file, permission, 0644);
    close(std_fd);
    dup2(fd, std_fd);
    close(fd);
    exec_cmd(cmd_arr);
}

static void exec_multi_cmd(const Cmd *cmd)
{
    int total_redir = cmd->total - 1;
    for (int i = 0; i < total_redir; ++i) {
        // start pipeline and redirection
        char **cmd1_arr = prep_cmd(cmd, i);
        if (cmd->redirect[i] == '|') {
            pipeline(cmd1_arr);
        } else if (cmd->redirect[i] == '>' || cmd->redirect[i] == '<') {
            char *filename = cmd->sets[i + 1];
            cmd->redirect[i] == '>' ? redir(cmd1_arr, filename, STDOUT_FILENO)
                                    : redir(cmd1_arr, filename, STDIN_FILENO);
            ++i;
        }
    }
    if (cmd->redirect[total_redir - 1] == '|') {
        char **last_cmd_arr = prep_cmd(cmd, total_redir);
        exec_cmd(last_cmd_arr);
    }
}

static int daemon_num = 0;
static void exec_background(const Cmd *cmd)
{
    printf("[%d]+ Running\t\t%s\n", ++daemon_num, cmd->sets[0]);
    if (cmd->total == 1)
        exec_uni_cmd(cmd);
    else if (cmd->total > 1)
        exec_multi_cmd(cmd);
    exit(0);
}

static bool is_sh_exit(const Cmd *cmd)
{
    for (int i = 0; i < cmd->total; ++i)
        if (is_exit(cmd->sets[i]))
            return true;
    return false;
}

void init_shell(Shell **self)
{
    if (!(*self = (Shell *)malloc(sizeof(Shell)))) {
        fprintf(stderr, "Failed to allocate memory in %s", __FUNCTION__);
        return;
    }
    (*self)->read_cmd = read_cmd;
    (*self)->exec_uni_cmd = exec_uni_cmd;
    (*self)->exec_multi_cmd = exec_multi_cmd;
    (*self)->exec_background = exec_background;
    (*self)->is_sh_exit = is_sh_exit;
}
