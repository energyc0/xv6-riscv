#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "file.h"

#define REG(reg) ((volatile uint32 *)(reg))

static struct sleeplock lock;

int
rtcread(int minor, int user_dst, uint64 dst, int n)
{
  if (n < sizeof(uint64))
    return -1;
  acquiresleep(&lock);
  uint32 timelow = *REG(RTC_TIME_LOW);
  uint32 timehigh = *REG(RTC_TIME_HIGH);
  releasesleep(&lock);
  uint64 res = (((uint64)timehigh) << 32) | ((uint64)timelow);
  if (either_copyout(user_dst, dst, &res, sizeof(res)) == 0) {
    return sizeof(uint64);
  } else {
    return -1;
  }
}

int
rtcwrite(int minor, int user_src, uint64 src, int n)
{
  return -1;
}

void
rtcinit()
{
  initsleeplock(&lock, "rtc_sleeplock");

  devsw[MAJOR_RTC].read = rtcread;
  devsw[MAJOR_RTC].write = rtcwrite;
}