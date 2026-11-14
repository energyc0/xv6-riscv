#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define READ_SIZE 64

static char hex_digits[] = "0123456789abcdef";

static int
parse_count(char *s, int *count)
{
  uint value = 0;
  int i;

  if (s[0] == '\0')
    return -1;
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] < '0' || s[i] > '9')
      return -1;
    if (value > (0x7fffffffU - (s[i] - '0')) / 10)
      return -1;
    value = value * 10 + (s[i] - '0');
  }
  *count = value;
  return 0;
}

static int
dump(int fd, int count)
{
  uchar buf[READ_SIZE];
  int n, remaining = count, printed = 0, i;

  while (remaining > 0) {
    int request = remaining;
    if (request > sizeof(buf))
      request = sizeof(buf);

    n = read(fd, buf, request);
    if (n < 0) {
      if (printed)
        printf("\n");
      fprintf(2, "Read error\n");
      return -1;
    }
    if (n == 0)
      break;

    for (i = 0; i < n; i++) {
      if (printed)
        printf(" ");
      printf("%c%c", hex_digits[buf[i] >> 4], hex_digits[buf[i] & 0xf]);
      printed = 1;
    }
    remaining -= n;
  }

  if (printed)
    printf("\n");
  return 0;
}

int
main(int argc, char *argv[])
{
  int fd, count;

  if (argc != 3 || parse_count(argv[1], &count) < 0) {
    fprintf(2, "usage: hexdump <count> <file>\n");
    exit(1);
  }

  fd = open(argv[2], O_RDONLY);
  if (fd < 0) {
    fprintf(2, "hexdump: cannot open %s\n", argv[2]);
    exit(1);
  }

  if (dump(fd, count) < 0) {
    close(fd);
    exit(1);
  }
  close(fd);
  exit(0);
}
