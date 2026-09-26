enum pinfostate { PI_SLEEPING, PI_RUNNABLE, PI_RUNNING, PI_ZOMBIE };

struct procinfo {
  int pid;
  int ppid;
  char name[16];
  enum pinfostate state;
};

