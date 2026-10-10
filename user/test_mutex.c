#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
put_c(char c)
{
  write(1, &c, 1);
}

void
put_s(char *s)
{
  while (*s) {
    put_c(*s++);
  }
}

void
put_d(int n)
{
  char buf[12];
  int i = 0;
  if (n == 0) {
    put_c('0');
    return;
  }
  while (n > 0) {
    buf[i++] = (n % 10) + '0';
    n /= 10;
  }
  for (int j = i - 1; j >= 0; j--) {
    put_c(buf[j]);
  }
}

void
print_row(int pid, int arg_num, char c)
{
  put_d(pid);
  put_s(": arg ");
  put_d(arg_num);
  put_s(", char '");
  put_c(c);
  put_s("'\n");
}

void
test_without_mutex(int argc, char *argv[])
{
  int pid = getpid();
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    for (int j = 0; arg[j] != '\0'; j++) {
      print_row(pid, i, arg[j]);
    }
  }
}

void
test_with_mutex(int fd, int argc, char *argv[])
{
  int pid = getpid();
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    for (int j = 0; arg[j] != '\0'; j++) {
      mutex_lock(fd);
      print_row(pid, i, arg[j]);
      mutex_unlock(fd);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd = mutex();
  int pid1 = fork();
  if (pid1 < 0) {
    printf("Error fork\n");
    exit(1);
  }

  test_without_mutex(argc, argv);

  if (pid1 > 0) {
    wait(0);

    printf("\nmutex\n");

    if (fd < 0) {
      printf("Error creating mutex\n");
      exit(1);
    }

    int pid2 = fork();
    if (pid2 < 0) {
      printf("Error fork\n");
      close(fd);
      exit(1);
    }

    test_with_mutex(fd, argc, argv);

    if (pid2 > 0) {
      wait(0);
      close(fd);
    }
  }

  exit(0);
}