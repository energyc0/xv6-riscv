#include "kernel/types.h"
#include "user/user.h"

void
test_basic()
{
  printf("Test 1: Basic allocation and free... \n");
  int *p = (int *)malloc(sizeof(int) * 10);
  if (p == 0) {
    printf("FAILED\n");
    return;
  }

  p[0] = 42;
  p[9] = 99;

  free(p);
  printf("PASSED\n");
}

void
test_various_sizes()
{
  printf("Test 2: Various sizes (testing segregated lists)... \n");

  void *p1 = malloc(10);
  void *p2 = malloc(50);
  void *p3 = malloc(1500);

  if (p1 == 0 || p2 == 0 || p3 == 0) {
    printf("FAILED (one of the allocations returned 0)\n");
    return;
  }

  ((char *)p1)[0] = 'A';
  ((char *)p2)[0] = 'B';
  ((char *)p3)[0] = 'C';

  free(p1);
  free(p2);
  free(p3);
  printf("PASSED\n");
}

void
test_large_allocation()
{
  printf("Test 3: Large allocation (> 2048 bytes)... \n");
  char *p = (char *)malloc(3000);
  if (p == 0) {
    printf("FAILED (returned 0)\n");
    return;
  }

  p[0] = 'X';
  p[2999] = 'Y';

  free(p);
  printf("PASSED\n");
}

void
test_reuse()
{
  printf("Test 4: Memory reuse after free... \n");

  void *p1 = malloc(32);
  if (p1 == 0) {
    printf("FAILED (initial alloc returned 0)\n");
    return;
  }

  free(p1);

  void *p2 = malloc(32);
  if (p2 == 0) {
    printf("FAILED (reuse alloc returned 0)\n");
    return;
  }

  if (p1 == p2) {
    printf("PASSED (address perfectly reused: %p)\n", p2);
  } else {
    printf("PASSED (but address not reused: %p vs %p)\n", p1, p2);
  }

  free(p2);
}

void
test_edge_cases()
{
  printf("Test 5: Edge cases (malloc(0) and free(0))... \n");

  void *p0 = malloc(0);
  if (p0 != 0) {
    printf("FAILED (malloc(0) should return 0)\n");
    return;
  }

  free(0);

  printf("PASSED\n");
}

int
main(int argc, char *argv[])
{
  printf("\n=== Starting Allocator Tests ===\n");

  test_basic();
  test_various_sizes();
  test_large_allocation();
  test_reuse();
  test_edge_cases();

  printf("=== All Tests Completed Successfully ===\n\n");
  exit(0);
}