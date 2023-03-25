#include "headers/utils.h"
#include "headers/cmd.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_redir_sign(char c)
{
    return c == '|' || c == '<' || c == '>' || c == '&';
}

bool is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

char *trim(char *str)
{
    char *end = str + strlen(str);
    while (is_whitespace(*str)) {
        ++str;
    }
    while (is_whitespace(*end)) {
        *end = '\0';
        --end;
    }
    return str;
}

int calc_cmd_frag(const char *ptr)
{
    char c;
    int i = 1;
    bool pre_space = false;

    while ((c = *ptr++))
        if (c == ' ' && !pre_space)
            ++i, pre_space = true;
        else if (c == ' ')
            continue;
        else
            pre_space = false;

    return i;
}

void init_str_arr(char **ptr, const int arr_len, const int str_len)
{
    for (int i = 0; i < arr_len; i++) {
        ptr[i] = (char *)malloc(str_len);
        memset(ptr[i], '\0', str_len);
    }
}

void free_str_arr(char **ptr, const int arr_len)
{
    for (int i = 0; i < arr_len; i++)
        free(ptr[i]);
    free(ptr);
}

void split_cmd(char *cmd_str, char **arr)
{
    int i = 0;
    char *cmd_frag = strtok(cmd_str, " ");

    // NULL is required to be the last element of the execvp() argument
    while (1) {
        arr[i] = cmd_frag;
        if (cmd_frag == NULL)
            break;

        cmd_frag = strtok(NULL, " ");
        ++i;
    }
#ifdef DEBUG_SPLIT_CMD
    FILE *fp = fopen("debug.log", "a");
    for (int j = 0; j < i; ++j)
        fprintf(fp, "cmd_arr[%d]: %s\n", j, arr[j]);
    fclose(fp);
#endif
}

int resize_cmd_len(const int cur_num, const int cur_len, const Cmd *cmd,
                   int max_cmd_len)
{
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

int resize_cmd_set(const int cur_num, Cmd *cmd, int max_cmd_len)
{
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
