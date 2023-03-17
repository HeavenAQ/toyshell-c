#ifndef MY_SHELL_H
#define MY_SHELL_H
#include <stdbool.h>
#include <string.h>

#include "cmd.h"
typedef struct Shell Shell;
struct Shell {
    void (*read_cmd)(Cmd *cmd);
    void (*exec_uni_cmd)(const Cmd *cmd);
    void (*exec_multi_cmd)(const Cmd *cmd);
};
void init_shell(Shell **self);
#endif
