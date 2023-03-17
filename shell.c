#include "shell.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

static int resize_cmd_len(const int cur_num, const int cur_len, const Cmd *cmd,
                          int max_cmd_len) {
    char *cur_end = cmd->sets[cur_num] + cur_len;
    max_cmd_len += CMD_LEN;
    cmd->sets[cur_num] = realloc(cmd->sets[cur_num], max_cmd_len);

    if (!cmd->sets[cur_num]) {
        DBG("Failed to realloc");
        return -1;
    }
    memset(cur_end, '\0', CMD_LEN);
    return max_cmd_len;
}

static int resize_cmd_set(const int cur_num, Cmd *cmd, int max_cmd_len) {
    // resize redirect symbol array
    char *cur_end = cmd->redirect + cur_num;
    max_cmd_len += CMD_NUM;
    cmd->redirect = realloc(cmd->redirect, max_cmd_len);
    if (!cmd->redirect) {
        DBG("Failed to realloc");
        return -1;
    }
    memset(cur_end, '\0', CMD_NUM);

    // resize command set array
    cmd->sets = realloc(cmd->sets, max_cmd_len * sizeof(char *));
    if (!cmd->sets) {
        DBG("Failed to realloc");
        return -1;
    }
    init_str_arr(cmd->sets + cur_num, max_cmd_len, CMD_LEN);
    return max_cmd_len;
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
    for (int k = 0; k < cur_num; ++k) {
        cmd->sets[k] = trim(cmd->sets[k]);
    }
}

static bool is_exit(char *cmd) { return strcmp(cmd, "exit\n") == 0; }

void init_shell(Shell **self) {
    if (!(*self = (Shell *)malloc(sizeof(Shell)))) {
        fprintf(stderr, "Failed to allocate memory in %s", __FUNCTION__);
        return;
    }
    (*self)->read_cmd = read_cmd;
    (*self)->is_exit = is_exit;
}
