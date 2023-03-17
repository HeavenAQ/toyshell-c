#ifndef MY_SHELL_H
#define MY_SHELL_H
#include <stdbool.h>

#include "cmd.h"
typedef struct Shell Shell;
struct Shell {
    void (*read_cmd)(Cmd *cmd);
    bool (*is_exit)(char *cmd);
};
void init_shell(Shell **self);
#endif
