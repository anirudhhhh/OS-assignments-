// User program to test virtual-to-physical address translation (vtop)
// Question 2: Virtual Memory: Virtual-to-Physical Address Translation

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

static void test_address(const char *desc, uint64 va)
{
  uint64 pa = vtop(va);
  if (pa == (uint64)-1) {
    printf("[UNMAPPED/INVALID] %s: VA %p -> Not Mapped (Error -1)\n", desc, (void *)va);
  } else {
    printf("[MAPPED] %s: VA %p -> PA %p (Offset: 0x%x)\n",
           desc, (void *)va, (void *)pa, (uint)(va & (PGSIZE - 1)));
  }
}

int main(int argc, char *argv[])
{
  printf("=== Question 2: Virtual-to-Physical Address Translation ===\n\n");

  printf("1. Translating Code & Stack Virtual Addresses:\n");
  test_address("Code Segment (main)", (uint64)main);

  int stack_var = 42;
  test_address("Stack Variable", (uint64)&stack_var);

  printf("\n2. Allocating 3 Pages via sbrk()...\n");
  char *p = sbrk(3 * PGSIZE);
  if (p == (char *)-1) {
    printf("vttest: sbrk failed\n");
    exit(1);
  }
  // Initialize allocated memory
  p[0] = 'X';
  p[PGSIZE] = 'Y';
  p[2 * PGSIZE] = 'Z';

  printf("Allocated 3 pages starting at VA %p\n", p);
  test_address("Allocated Page 0 Base", (uint64)p);
  test_address("Allocated Page 0 + Offset 0x555", (uint64)p + 0x555);
  test_address("Allocated Page 1 Base", (uint64)(p + PGSIZE));
  test_address("Allocated Page 2 + Offset 0x888", (uint64)(p + 2 * PGSIZE + 0x888));

  printf("\n3. Testing Unmapped and Invalid Virtual Addresses:\n");
  test_address("Unmapped Beyond Heap", (uint64)(p + 100 * PGSIZE));
  test_address("Stack Guard Page (0x2000)", 0x2000);
  test_address("Invalid High Address (0x8000000000)", 0x8000000000ULL);

  printf("\nvttest completed successfully.\n");
  exit(0);
}
