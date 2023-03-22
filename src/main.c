#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "headers/cmd.h"
#include "headers/shell.h"
#include "headers/utils.h"

void shell_loop(Shell *sh) {
    while (1) {
        Cmd *cmd = NULL;
        init_cmd(&cmd);

        printf("h-sh> ");
        sh->read_cmd(cmd);
        fflush(stdin);

#ifdef DEBUG_READ_CMD
        FILE *fp = fopen("debug.log", "a");
        fprintf(fp, "total: %d\n", cmd->total);
        fprintf(fp, "redirect: %s\n", cmd->redirect);
        fprintf(fp, "is_background: %d\n", cmd->is_background);
        for (int i = 0; i < cmd->total; ++i)
            fprintf(fp, "cmd string: %s\n", cmd->sets[i]);
        fclose(fp);
#endif
        if (cmd->is_background)
            fork() == 0 ? sh->exec_background(cmd) : NULL;
        else if (cmd->total == 1)
            fork() != 0 ? wait(NULL) : sh->exec_uni_cmd(cmd);
        else if (cmd->total > 1)
            fork() != 0 ? wait(NULL) : sh->exec_multi_cmd(cmd);

        // exit if the command is "exit"
        if (sh->is_sh_exit(cmd)) {
            // clean up
            free_cmd(&cmd);
            cmd = NULL;
            break;
        }

        // clean up
        free_cmd(&cmd);
        cmd = NULL;
    }
}

int main(void) {
    Shell *sh = NULL;
    init_shell(&sh);
    fork() != 0 ? wait(NULL), free(sh) : shell_loop(sh);
}
