#ifndef MY_CMD_H
#define MY_CMD_H
#include <stdbool.h>
#define KB *1024
#define CMD_LEN 1 KB
#define CMD_NUM 5

typedef struct Cmd Cmd;
struct Cmd {
    char **sets;
    char *redirect;
    int total;
    bool is_background;
};

void init_cmd(Cmd **cmd);
void free_cmd(Cmd **cmd);
#endif
