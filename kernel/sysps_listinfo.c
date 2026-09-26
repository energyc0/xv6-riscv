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
        struct proc *p = &proc[i];
        acquire(&p->lock);
        if(p->state == UNUSED || p->state == USED) {
            release(&p->lock);
            continue;
        }
        struct procinfo info;
        info.pid = p->pid;
        info.state = p->state;
        safestrcpy(info.name, p->name, sizeof(p->name));

        struct proc *parent = p->parent;
        if (parent != 0) {
            acquire(&parent->lock);
            info.ppid = parent->pid;
            release(&parent->lock);
        } else {
            info.ppid = 0;
        }
        if (copyout(cur_proc->pagetable,cur_proc->sz, (uint64)(plist + count),(char*)&info,  sizeof(info)))
            return -3;
        release(&p->lock);
        count++;
    }
    release(&wait_lock);

    return 0;
}