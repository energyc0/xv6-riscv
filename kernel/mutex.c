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
  mtx->ref = 1;
  mtx->readable = 0;
  mtx->writable = 0;
  mtx->lock = (struct sleeplock *)kalloc();
  initsleeplock(mtx->lock, "Mutex");

  return mtx;
}
void
mutexclose(mutex mtx)
{
  kfree(mtx->lock);
}