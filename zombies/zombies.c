#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void check_zombies() {
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    while (pid > 0) {
        printf("[PID] %d", pid);
        if (WIFEXITED(status)) {
            printf("   DONE status: %d\n", WEXITSTATUS(status));
        } else {
            printf("   DONE\n");
        }
        pid = waitpid(-1, &status, WNOHANG);
    }
}