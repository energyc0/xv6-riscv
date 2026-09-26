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
    argint(1, &lim);

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
            continue;
        struct procinfo info;
        info.pid = proc[i].pid;
        info.state = proc[i].state;
        safestrcpy(info.name, proc[i].name, sizeof(proc[i].name));

        acquire(&proc[i].parent->lock);
        printk("Good!\n");
        info.ppid = proc[i].parent->pid;
        release(&proc[i].parent->lock);
        printk("Good!\n");
        if (copyout(cur_proc->pagetable,cur_proc->sz, (uint64)(plist + count),(char*)&info,  sizeof(info)))
            return -3;
        release(&proc[i].lock);
        count++;
    }
    release(&wait_lock);

    return 0;
}