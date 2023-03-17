#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/types.h>
#include <unistd.h>

#include "headers/cmd.h"
#include "headers/shell.h"
#include "headers/utils.h"

void shell_loop(Shell *sh) {
    while (1) {
        // get ready to receive command
        Cmd *cmd = NULL;
        init_cmd(&cmd);

        sh->read_cmd(cmd);

#ifdef DEBUG_READ_CMD
        printf("redirect: %s\n", cmd->redirect);
        for (int i = 0; i < cmd->total; ++i)
            puts(cmd->sets[i]);
#endif
        switch (cmd->total) {
        case 0:
            continue;
            break;
        case 1:
            sh->exec_uni_cmd(cmd);
            break;
        default:
            sh->exec_multi_cmd(cmd);
            break;
        }

        /*sh->is_exit(sh, cmd->sets) ? exit(0) : NULL;*/
        free_cmd(&cmd);
    }
}

int main(void) {
    Shell *sh = NULL;
    init_shell(&sh);
    fork() != 0 ? wait(NULL), free(sh) : shell_loop(sh);
}
