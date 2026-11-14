#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "riscv.h"
#include "defs.h"

static struct { // global state of driver
  struct spinlock lock;
  uint64 nullstat_bytes_written; // total amount written to nullstat
} pseudo;

static int
pseudo_dev_read(int minor, int user_dst, uint64 dst, int n)
{
  if (n < 0)
    return -1;

  switch (minor) {
  case MINOR_PSEUDO_NULL:
    return 0; // eof

  case MINOR_PSEUDO_NULLSTAT: {
    if (n != (int)sizeof(uint64))
      return -1;

    uint64 value; // пока пишем в буфер, держим на локе
    acquire(&pseudo.lock);
    value = pseudo.nullstat_bytes_written;
    release(&pseudo.lock);

    if (
      either_copyout(user_dst, dst, &value, sizeof(value)) <
      0) // возвращает общее кол-во прочитанных байт если равно кол-ву запрошенных
      return -1;
    return (int)sizeof(value);
  }

  case MINOR_PSEUDO_ZERO: { // возвращаем нулевые байты
    char zeros[64];
    memset(zeros, 0, sizeof(zeros));

    int done = 0;
    while (done < n) {
      int chunk = n - done;
      if (chunk > (int)sizeof(zeros))
        chunk = (int)sizeof(zeros);

      if (either_copyout(user_dst, dst + done, zeros, chunk) < 0)
        return -1;
      done += chunk;
    }
    return n;
  }

  default:
    return -1;
  }
}

static int
pseudo_dev_write(int minor, int user_src, uint64 src, int n)
{
  (void)user_src;
  (void)src;

  if (n < 0)
    return -1;

  switch (minor) {
  case MINOR_PSEUDO_NULL: //  игнорироваение
    return n;

  case MINOR_PSEUDO_NULLSTAT: // отбрасывает байты, ссчитаем кол=-во
    acquire(&pseudo.lock);
    pseudo.nullstat_bytes_written += (uint64)n;
    release(&pseudo.lock);
    return n;

  case MINOR_PSEUDO_ZERO: // запись не допускается
    return -1;

  default:
    return -1;
  }
  return -1;
}

void
pseudo_dev_init(void)
{

  // connect read and write system calls
  initlock(&pseudo.lock, "pseudo");
  devsw[MAJOR_PSEUDO_DEV].read = pseudo_dev_read;
  devsw[MAJOR_PSEUDO_DEV].write = pseudo_dev_write;
}