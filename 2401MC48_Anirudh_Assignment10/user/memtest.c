// User program to test physical memory tracking (memstats)
// Question 1: Memory Management: Tracking Physical Memory Allocation

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PAGES 10
#define PGSIZE 4096

int main(int argc, char *argv[])
{
  printf("=== Question 1: Tracking Physical Memory Allocation ===\n\n");

  printf("1. Baseline Memory Statistics (Before Allocation):\n");
  memstats();

  printf("\n2. Allocating %d pages (%d bytes) using sbrk()...\n", NUM_PAGES, NUM_PAGES * PGSIZE);
  char *ptr = sbrk(NUM_PAGES * PGSIZE);
  if (ptr == (char *)-1) {
    printf("memtest: sbrk failed\n");
    exit(1);
  }

  // Touch each allocated page
  for (int i = 0; i < NUM_PAGES; i++) {
    ptr[i * PGSIZE] = (char)('A' + (i % 26));
  }
  printf("Successfully allocated and touched %d pages at virtual address %p\n", NUM_PAGES, ptr);

  printf("\n3. Memory Statistics After Allocation:\n");
  memstats();

  printf("\n4. Releasing %d pages using sbrk(-%d)...\n", NUM_PAGES, NUM_PAGES * PGSIZE);
  if (sbrk(-NUM_PAGES * PGSIZE) == (char *)-1) {
    printf("memtest: sbrk deallocation failed\n");
    exit(1);
  }
  printf("Successfully released %d pages\n", NUM_PAGES);

  printf("\n5. Memory Statistics After Deallocation:\n");
  memstats();

  printf("\nmemtest completed successfully.\n");
  exit(0);
}
