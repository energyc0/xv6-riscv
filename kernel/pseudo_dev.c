#include "types.h"
#include "defs.h"
#include "file.h"

static uint64 nullstat_bytes_written = 0;

int
pseudo_dev_read(int minor, int user_src, uint64 src, int n)
{
  switch (minor) {
  case MINOR_PSEUDO_NULL: {
    return EOF;
  }
  case MINOR_PSEUDO_NULLSTAT: {
    if (n == sizeof(uint64)) {
      return nullstat_bytes_written;
    } else {
      return -1;
    }
  }
  case MINOR_PSEUDO_ZERO: {
    char *src_data = (char *)src;
    for (int i = 0; i < n; i++) {
      src_data[i] = 0;
    }
    return n;
  }
  default:
    return -1;
  }
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