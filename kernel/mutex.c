#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

mutex
mutexalloc()
{
  mutex mtx = filealloc();
  mtx->type = FD_MUTEX;
  mtx->ref = 1;
  mtx->readable = 0;
  mtx->writable = 0;
  mtx->type = FD_MUTEX;
  mtx->lock = (struct sleeplock *)kalloc();
  initsleeplock(mtx->lock, "Mutex");

  printk("PID %d: Allocated mutex - %p\n", myproc()->pid, mtx);

  return mtx;
}
void
mutexclose(mutex mtx)
{
  kfree(mtx->lock);

  printk("PID %d: Released mutex lock - %p\n", myproc()->pid, mtx);
}