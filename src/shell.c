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
static inline void exec_cmd(char *const *cmd_arr) {
    is_exit(cmd_arr[0]) ? exit(0) : NULL;
    if (is_cd(cmd_arr[0])) {
        chdir(cmd_arr[1]);
        return;
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

static char **prep_cmd(const Cmd *cmd, const int i) {
    char *cmd_str = cmd->sets[i];
    char **cmd_arr = malloc(sizeof(char *) * CMD_NUM);

    // split the command into an array
    int cmd_frag_num = calc_cmd_frag(cmd_str);
    init_str_arr(cmd_arr, cmd_frag_num, CMD_LEN);
    split_cmd(cmd_str, cmd_arr);
    return cmd_arr;
}

static void read_cmd(Cmd *cmd) {
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

static void exec_uni_cmd(const Cmd *cmd) {
    // split the command into an array
    char **cmd_arr = prep_cmd(cmd, 0);
    exec_cmd(cmd_arr);
}

static void pipeline(char **cmd1, char **cmd2) {
    int fd[2];
    pipe(fd);
    if (fork() != 0) {
        close(fd[0]);
        close(STDOUT_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        exec_cmd(cmd1);
    } else {
        close(fd[1]);
        close(STDIN_FILENO);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        exec_cmd(cmd2);
    }
}

static void redir_out(char **cmd_arr, char *const *file) {
    int fd = open(file[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    close(STDOUT_FILENO);
    dup2(fd, STDOUT_FILENO);
    close(fd);
    exec_cmd(cmd_arr);
}

static void redir_in(char **cmd_arr, char *const *file) {
    int fd = open(file[0], O_RDONLY);
    close(STDIN_FILENO);
    dup2(fd, STDIN_FILENO);
    close(fd);
    exec_cmd(cmd_arr);
}

static void exec_multi_cmd(const Cmd *cmd) {
    for (int i = 0; i < cmd->total; ++i) {
        // start pipeline and redirection
        char **cmd1_arr = prep_cmd(cmd, i);
        char **cmd2_arr = prep_cmd(cmd, i + 1);
        if (cmd->redirect[i] == '|')
            pipeline(cmd1_arr, cmd2_arr);

        cmd->redirect[i] == '>' ? redir_out(cmd1_arr, cmd2_arr)
                                : redir_in(cmd1_arr, cmd2_arr);

        // clean up
        free(cmd1_arr);
        free(cmd2_arr);
    }
}
static void exec_background(const Cmd *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Failed to fork in %s", __FUNCTION__);
        return;
    } else if (pid != 0) {
        return;
    }

    // make this a daemon process
    int fd = open("/dev/null", O_RDWR);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (cmd->total == 1)
        exec_uni_cmd(cmd);
    else if (cmd->total > 1)
        exec_multi_cmd(cmd);
}

static bool is_sh_exit(const Cmd *cmd) {
    for (int i = 0; i < cmd->total; ++i)
        if (is_exit(cmd->sets[i]))
            return true;
    return false;
}

static void prompt() {
    char *user = getenv("USER");
    char *host = getenv("HOSTNAME");
    char pwd[PATH_MAX];
    getcwd(pwd, sizeof(pwd));
    printf("%s@%s:%s > ", user, host, pwd);
}

void init_shell(Shell **self) {
    if (!(*self = (Shell *)malloc(sizeof(Shell)))) {
        fprintf(stderr, "Failed to allocate memory in %s", __FUNCTION__);
        return;
    }
    (*self)->prompt = prompt;
    (*self)->read_cmd = read_cmd;
    (*self)->exec_uni_cmd = exec_uni_cmd;
    (*self)->exec_multi_cmd = exec_multi_cmd;
    (*self)->exec_background = exec_background;
    (*self)->is_sh_exit = is_sh_exit;
}
