#include "kernel/procinfo.h"
#include "kernel/types.h"
#include "user/user.h"

#define PROC_LIST_SIZE 16

const char* pstate_names[] = {
    [UNUSED] = "UNUSED", 
    [USED] = "USED",
    [SLEEPING] = "SLEEPING",
    [RUNNABLE] = "RUNNABLE",
    [RUNNING] = "RUNNING",
    [ZOMBIE] = "ZOMBIE"
};

int main(int argc, char** argv)
{
    struct procinfo plist[PROC_LIST_SIZE];
    int k = ps_listinfo(plist, PROC_LIST_SIZE);
    if (k < 0)
        printf("Error!\n");
    else {
        printf("%d\n", k);
        for (int i = 0; i < k; i++) {
            printf("PID: %d, NAME: %s, STATE: %s, PPID: %d\n",
            plist[i].pid, plist[i].name, pstate_names[plist[i].state], plist[i].ppid);
        }
    }
    return 0;
}