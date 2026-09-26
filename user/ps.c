#include "kernel/procinfo.h"
#include "kernel/types.h"
#include "user/user.h"

#define PROC_LIST_SIZE 16

const char* pstate_names[] = {
    [PI_SLEEPING] = "SLEEPING",
    [PI_RUNNABLE] = "RUNNABLE",
    [PI_RUNNING] = "RUNNING",
    [PI_ZOMBIE] = "ZOMBIE"
};

int main(int argc, char** argv)
{
    int start_size = 0;
    int k = 0;
    while (k == start_size) {
        start_size += PROC_LIST_SIZE;
        struct procinfo plist[PROC_LIST_SIZE];
        k = ps_listinfo(plist, PROC_LIST_SIZE);
        if (k < 0) {
            fprintf(2, "Failed to read process list!\n");
            return 1;
        }
        printf("Processes count: %d\n", k);
        for (int i = 0; i < k; i++) {
            printf("PID: %d, NAME: %s, STATE: %s, PPID: %d\n",
            plist[i].pid, plist[i].name, pstate_names[plist[i].state], plist[i].ppid);
        }
        break;
    }
    return 0;
}