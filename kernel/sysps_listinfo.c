#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "procinfo.h"


extern struct spinlock wait_lock;
extern struct proc proc[NPROC];
/* 
    Return amount of processes in the list
*/
uint64 sys_ps_listinfo(void)
{
    struct procinfo *plist;
    int lim;
    argaddr(0, (uint64*)&plist);
    argint(0, &lim);

    if (plist == 0 || lim < 0)
        return -1;
    
    struct proc *cur_proc = myproc();
    if(cur_proc == 0)
        return -2;


    acquire(&wait_lock);
    uint64 count = 0;
    for(int i = 0; i < NPROC && count < lim; i++) {
        acquire(&proc[i].lock);
        if(proc[i].state == UNUSED || proc[i].state == USED)
            i++;
        if (copyout(cur_proc->pagetable,cur_proc->sz, (uint64)(plist + count),(char*)&proc[i],  sizeof(proc)))
            return -3;
        release(&proc[i].lock);
    }
    release(&wait_lock);

    return 0;
}