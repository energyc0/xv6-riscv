#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAUSE_SEC(s) (s*10)

int main(int argc, char** argv) {
    if (argc != 2 || (strcmp("-a", argv[1]) != 0 && strcmp("-b", argv[1]) != 0)) {
        fprintf(2, "Expected options '-a' or '-b'.\nUsage: %s [-a/-b]\n", argv[0]);
        return 1;
    }
    int is_b_option = strcmp(argv[1], "-b") == 0;

    int ret = fork();
    if (ret == 0) {
        pause(PAUSE_SEC(10));
        exit(1);
    } else if (ret > 0) {
        if (is_b_option)
            kill(ret);
        int wstatus = 0;
        if (wait(&wstatus) == -1) {
            fprintf(2, "wait(): failed\n");
        }
        int pid = getpid();
        printf("Main process` pid: %d, child process` pid: %d, child process` exit code: %d\n",
             pid, ret, wstatus);
    } else {
        fprintf(2,"fork(): failed\n");
    }
    return 0;
}