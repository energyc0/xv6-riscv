#include "types.h"

struct procinfo {
  int pid;
  int ppid;
  char name[16];
  enum procstate state;
};

