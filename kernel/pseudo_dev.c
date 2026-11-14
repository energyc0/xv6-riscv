#include "types.h"
#include "defs.h"
#include "file.h"

#define

static int
nullread(int user_dst, uint64 dst, int n)
{
}

static int
zeroread(int user_dst, uint64 dst, int n)
{
}

static int
nullstatread(int user_dst, uint64 dst, int n)
{
}

int
pseudo_dev_read(int minor, int user_src, uint64 src, int n)
{
  switch (minor) {
  case MINOR_PSEUDO_NULL: {
    char *src_data = (char *)src;
    for (int i = 0; i < n; i++) {
      src_data[i] = EOF;
    }
    break;
  }
  case MINOR_PSEUDO_NULLSTAT: {
    if (n == sizeof(uint64)) {
    } else {
      return -1;
    }
    break;
  }
  case MINOR_PSEUDO_ZERO:
    break;
  default:
    return -1;
  }
  return n;
}

static int
nullwrite(int user_src, uint64 src, int n)
{
}

static int
zerowrite(int user_src, uint64 src, int n)
{
}

static int
nullstatwrite(int user_src, uint64 src, int n)
{
}

int
pseudo_dev_write(int minor, int user_dst, uint64 dst, int n)
{
  switch (minor) {
  case MINOR_PSEUDO_NULL:
    break;
  case MINOR_PSEUDO_NULLSTAT:
    break;
  case MINOR_PSEUDO_ZERO:
    break;
  default:
    return -1;
  }
}

void
pseudo_dev_init(void)
{
}