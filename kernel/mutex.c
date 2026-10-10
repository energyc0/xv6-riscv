#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#define NLOCKPTABLES     4
#define MUTEXPTABLECOUNT (PGSIZE / sizeof(struct sleeplock))
static struct sleeplock *
sleeplockalloc()
{
  static struct sleeplock *lock_ptables[NLOCKPTABLES];
  static int n_ptables = 0;
  static int n_mutexes = MUTEXPTABLECOUNT;
  if (n_mutexes >= MUTEXPTABLECOUNT) {
    if (n_ptables >= NLOCKPTABLES)
      return 0;
    if ((lock_ptables[n_ptables++] = kalloc()) == 0)
      return 0;

    n_mutexes = 0;
  }

  return &(lock_ptables[n_ptables - 1][n_mutexes++]);
}

mutex
mutexalloc()
{
  mutex mtx = filealloc();
  mtx->type = FD_MUTEX;
  mtx->ref = 1;
  mtx->readable = 0;
  mtx->writable = 0;
  mtx->type = FD_MUTEX;
  mtx->lock = sleeplockalloc();
  if (mtx->lock == 0) {
    printk("PID %d: Failed to allocate mutex\n", myproc()->pid);
    return 0;
  }
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