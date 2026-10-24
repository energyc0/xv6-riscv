#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
test_kalloc()
{
  memory_pages();
  printf("\n");
  int pages_count = 3;
  int size = pages_count * 4096;
  char *p = sbrk(size);
  if (p == (char *)-1) {
    printf("Allocate error\n");
    exit(1);
  }

  for (int i = 0; i < pages_count; i++) {
    p[i * 4096] = 'm';
  }

  printf("PAGE TABLES\n");
  memory_pages();
  printf("\n");

  printf("11");

  if (sbrk(-size) == (char *)-1) {
    printf("Sweep error\n");
    exit(1);
  }
  printf("22");
  memory_pages();
  printf("\n");
}

int
main(int argc, char *argv[])
{
  test_kalloc();
  printf("kfoe");
  return 0;
}