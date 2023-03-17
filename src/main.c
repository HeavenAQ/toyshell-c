#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/types.h>
#include <unistd.h>

#include "headers/cmd.h"
#include "headers/shell.h"

void shell_loop(Shell *sh) {
    while (1) {
        // get ready to receive command
        Cmd *cmd = NULL;
        init_cmd(&cmd);

        sh->read_cmd(cmd);

        /*sh->is_exit(sh, cmd->sets) ? exit(0) : NULL;*/
        free_cmd(&cmd);
    }
}

int main(void) {
    Shell *sh = NULL;
    init_shell(&sh);
    pid_t pid;

    pid = fork();
    pid != 0 ? wait(NULL) : shell_loop(sh);
}
