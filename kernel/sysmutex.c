#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

static struct file*
argmutex(int n)
{
    struct file* f;

    if (argfd(n, 0, &f) < 0) {
        return 0;
    }

    if (f->type != FD_MUTEX) {
        return 0;
    }

    return f;
}

uint64
sys_mutex(void)
{
    struct file* mutex = mutexalloc();
    if (!mutex) {
        return -1;
    }

    int fd = fdalloc(mutex);
    if (fd < 0) {
        fileclose(mutex);
        return -1;
    }

    return fd;
}

uint64
sys_mutex_lock(void)
{
    struct file* f = argmutex(0);

    if (!f) {
        return 0;
    }

    if (holdingsleep(f->mtx)) {
        return -1;
    }

    acquiresleep(f->mtx);
    return 0;
}

uint64
sys_mutex_unlock(void)
{
    struct file* f = argmutex(0);
    
    if (!f) {
        return 0;
    }

    if (!holdingsleep(f->mtx)) {
        return -1;
    }

    releasesleep(f->mtx);
    return 0;
}