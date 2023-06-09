#include "headers/cmd.h"

#include "headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_cmd(Cmd **cmd)
{
    if (!(*cmd = (Cmd *)malloc(sizeof(Cmd)))) {
        fprintf(stderr, "Failed to allocate memory in %s", __FUNCTION__);
        return;
    }

    (*cmd)->redirect = (char *)malloc(CMD_LEN);
    memset((*cmd)->redirect, '\0', CMD_LEN);

    (*cmd)->sets = (char **)malloc(CMD_NUM * sizeof(char *));
    init_str_arr((*cmd)->sets, CMD_NUM, CMD_LEN);
    (*cmd)->total = 0;
    (*cmd)->is_background = false;
}

void free_cmd(Cmd **cmd)
{
    free((*cmd)->redirect);
    free((*cmd)->sets);
    free(*cmd);
    (*cmd)->redirect = NULL;
    (*cmd)->sets = NULL;
    (*cmd) = NULL;
}
