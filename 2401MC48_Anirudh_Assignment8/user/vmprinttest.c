#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Question 3: Process Page Table Hierarchy Dump\n\n");

  // Call 1: At process startup
  printf("=== 1. Page Table Dump at Startup ===\n");
  vmprint();

  // Call 2: After growing heap by 8 pages
  printf("\nGrowing heap by 8 pages (sbrk(8 * 4096))...\n");
  char *p = sys_sbrk(8 * 4096, 1);
  if (p == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }
  for (int i = 0; i < 8; i++) {
    p[i * 4096] = 'V';
  }

  printf("\n=== 2. Page Table Dump After Growing Heap (8 pages added) ===\n");
  vmprint();

  // Call 3: After shrinking heap by 1 page
  printf("\nShrinking heap by 1 page (sbrk(-4096))...\n");
  if (sys_sbrk(-4096, 1) == (char *)-1) {
    printf("sbrk shrink failed\n");
    exit(1);
  }

  printf("\n=== 3. Page Table Dump After Shrinking Heap (1 page removed) ===\n");
  vmprint();

  printf("\nvmprinttest completed successfully.\n");
  exit(0);
}
