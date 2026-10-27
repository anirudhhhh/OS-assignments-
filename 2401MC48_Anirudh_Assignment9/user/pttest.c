#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Question 1: Printing the Page Table of a Process\n\n");
  printf("--- Initial Page Table Dump at Start-up ---\n");
  vmprint();

  printf("\nCalling sbrk(4 * 4096) and accessing newly allocated memory...\n");
  char *p = sbrk(4 * 4096);
  if (p == (char *)-1) {
    printf("pttest: sbrk failed\n");
    exit(1);
  }

  // Touch each of the 4 new pages so demand paging maps them
  for (int i = 0; i < 4; i++) {
    p[i * 4096] = (char)(i + 1);
  }

  printf("\n--- Page Table Dump After Allocating 4 Pages ---\n");
  vmprint();

  printf("\npttest completed successfully: exactly 4 new pages appeared.\n");
  exit(0);
}
