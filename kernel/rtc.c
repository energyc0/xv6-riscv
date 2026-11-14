#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "sleeplock.h"
#include "defs.h"

#define REG(reg) ((volatile uint32 *)(reg))

static struct sleeplock lock;

int
rtcread(int minor, int user_src, uint64 src, int n)
{
  if (n != 2)
    return -1;
  acquiresleep(&lock);
  uint32 timelow = *REG(RTC_TIME_LOW);
  uint32 timehigh = *REG(RTC_TIME_HIGH);
  releasesleep(&lock);
  *((uint64 *)src) = (((uint64)timehigh) << 32) | ((uint64)timelow);
  return 2;
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
}