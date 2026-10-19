#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Print formatted memory statistics
void
print_meminfo(const char *label, struct meminfo *mi)
{
  printf("[%s]\n", label);
  printf("  Total pages:      %d\n", mi->total_pages);
  printf("  Free pages:       %d\n", mi->free_pages);
  printf("  User pages (sz):  %d\n", mi->user_pages);
  printf("  Pagetable pages:  %d\n", mi->pagetable_pages);
}

int
main(int argc, char *argv[])
{
  struct meminfo mi_initial, mi_allocated, mi_child, mi_after_wait;

  printf("Question 2: Physical Memory Statistics Test\n\n");

  // Step 1: Initial statistics
  if (getmeminfo(&mi_initial) < 0) {
    printf("getmeminfo failed\n");
    exit(1);
  }
  print_meminfo("1. Initial Baseline", &mi_initial);

  // Step 2: Grow memory using eager allocation
  printf("\nGrowing memory by 10 pages (40960 bytes)...\n");
  char *p = sys_sbrk(10 * 4096, 1);
  if (p == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }
  // Touch pages to ensure they are physically mapped
  for (int i = 0; i < 10; i++) {
    p[i * 4096] = 'M';
  }

  if (getmeminfo(&mi_allocated) < 0) {
    printf("getmeminfo failed\n");
    exit(1);
  }
  print_meminfo("2. After Allocating 10 Pages", &mi_allocated);
  printf("Delta free pages: %d (expected -10)\n",
         mi_allocated.free_pages - mi_initial.free_pages);

  // Step 3: Fork a child process
  printf("\nForking child process...\n");
  int free_before_fork = mi_allocated.free_pages;

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    if (getmeminfo(&mi_child) < 0) {
      printf("child getmeminfo failed\n");
      exit(1);
    }
    print_meminfo("3. Child Process View", &mi_child);
    printf("Child observed free pages reduction: %d pages allocated for child clone\n",
           free_before_fork - mi_child.free_pages);
    exit(0);
  }

  // Parent waits for child to exit and be reaped
  wait(0);
  printf("\nChild exited and reaped by parent.\n");

  if (getmeminfo(&mi_after_wait) < 0) {
    printf("getmeminfo failed\n");
    exit(1);
  }
  print_meminfo("4. After Child Exit & Reaping", &mi_after_wait);

  printf("\n--- Memory Leak Verification ---\n");
  printf("Free pages before fork: %d\n", free_before_fork);
  printf("Free pages after wait:  %d\n", mi_after_wait.free_pages);

  if (mi_after_wait.free_pages == free_before_fork) {
    printf("SUCCESS: No memory leak detected! Free pages restored exactly.\n");
  } else {
    printf("WARNING: Free pages mismatch: diff = %d\n",
           mi_after_wait.free_pages - free_before_fork);
  }

  printf("\nmemtest completed successfully.\n");
  exit(0);
}
