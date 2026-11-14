#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NS_PER_SECOND 1000000000ULL
#define SECONDS_PER_DAY 86400ULL

static int
is_leap_year(int year)
{
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static int
days_in_month(int year, int month)
{
  static const int days[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
  };

  if (month == 2 && is_leap_year(year))
    return 29;
  return days[month - 1];
}

static void
print_two_digits(int value)
{
  if (value < 10)
    printf("0");
  printf("%d", value);
}

int
main(int argc, char *argv[])
{
  int fd, n;
  uint64 time_ns, seconds, days, fraction;
  uint64 seconds_today;
  int year, month, day, hour, minute, second;
  int place;

  if (argc != 1) {
    fprintf(2, "usage: date\n");
    exit(1);
  }

  if ((fd = open("rtc", O_RDONLY)) < 0) {
    fprintf(2, "date: cannot open rtc\n");
    exit(1);
  }
  n = read(fd, &time_ns, sizeof(time_ns));
  close(fd);
  if (n != sizeof(time_ns)) {
    fprintf(2, "date: cannot read rtc\n");
    exit(1);
  }

  seconds = time_ns / NS_PER_SECOND;
  fraction = time_ns % NS_PER_SECOND;
  days = seconds / SECONDS_PER_DAY;
  seconds_today = seconds % SECONDS_PER_DAY;

  year = 1970;
  while (days >= (uint64)(is_leap_year(year) ? 366 : 365)) {
    days -= is_leap_year(year) ? 366 : 365;
    year++;
  }

  month = 1;
  while (days >= (uint64)days_in_month(year, month)) {
    days -= days_in_month(year, month);
    month++;
  }
  day = days + 1;

  hour = seconds_today / 3600;
  minute = (seconds_today % 3600) / 60;
  second = seconds_today % 60;

  printf("%d-", year);
  print_two_digits(month);
  printf("-");
  print_two_digits(day);
  printf(" ");
  print_two_digits(hour);
  printf(":");
  print_two_digits(minute);
  printf(":");
  print_two_digits(second);
  printf(".");
  for (place = 100000000; place > 0; place /= 10)
    printf("%c", (int)('0' + (fraction / place) % 10));
  printf(" UTC\n");
  exit(0);
}
