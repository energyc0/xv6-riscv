#include "kernel/procinfo.h"
#include "user/user.h"

#define PROC_LIST_SIZE 16

int main(int argc, char** argv)
{
    struct procinfo plist[PROC_LIST_SIZE];
    int k = ps_listinfo(plist, PROC_LIST_SIZE);
    if (k < 0)
        printf("Error!\n");
    else
        printf("%d\n", k);
    return 0;
}