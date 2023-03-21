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
        Cmd *cmd = NULL;
        init_cmd(&cmd);

        sh->read_cmd(cmd);
        fflush(stdin);

#ifdef DEBUG_READ_CMD
        FILE *fp = fopen("debug.log", "a");
        fprintf(fp, "total: %d\n", cmd->total);
        fprintf(fp, "redirect: %s\n", cmd->redirect);
        for (int i = 0; i < cmd->total; ++i)
            fprintf(fp, "cmd string: %s", cmd->sets[i]);
        fclose(fp);
#endif

        if (cmd->total == 1)
            fork() != 0 ? wait(NULL) : sh->exec_uni_cmd(cmd);
        else if (cmd->total > 1)
            fork() != 0 ? wait(NULL) : sh->exec_multi_cmd(cmd);

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
