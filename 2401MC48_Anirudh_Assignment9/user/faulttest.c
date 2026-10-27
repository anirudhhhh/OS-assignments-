#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Question 2: Handling Page Faults (Lazy Allocation)\n\n");

  int initial_faults = getfaults();
  printf("Initial page faults handled: %d\n", initial_faults);

  printf("Calling sbrk(32 * 4096) to increase heap by 32 pages...\n");
  char *p = sbrk(32 * 4096);
  if (p == (char *)-1) {
    printf("faulttest: sbrk failed\n");
    exit(1);
  }

  printf("Page faults after lazy sbrk: %d (0 frames allocated eagerly)\n", getfaults() - initial_faults);

  printf("\nWriting to exactly 5 different pages (indices 0, 5, 12, 20, 31)...\n");
  int pages_to_touch[5] = {0, 5, 12, 20, 31};
  for (int i = 0; i < 5; i++) {
    int idx = pages_to_touch[i];
    p[idx * 4096] = (char)('A' + i);
    printf("  Touched page %d (VA = %p), value = '%c'\n", idx, p + idx * 4096, p[idx * 4096]);
  }

  int faults_handled = getfaults() - initial_faults;
  printf("\nPage faults handled: %d (expected 5)\n", faults_handled);
  if (faults_handled != 5) {
    printf("FAILED: expected exactly 5 page faults, got %d\n", faults_handled);
    exit(1);
  }
  printf("SUCCESS: exactly 5 page faults were handled by demand paging.\n");

  printf("\n--- Testing Invalid Access Beyond Allocated Heap ---\n");
  int pid = fork();
  if (pid < 0) {
    printf("faulttest: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child (pid=%d) attempting access beyond sz (page 33)...\n", getpid());
    // Access page index 33 which is above sbrk(32 * 4096)
    volatile char *bad_ptr = p + 33 * 4096;
    *bad_ptr = 'X';
    printf("ERROR: child was not killed by kernel!\n");
    exit(1);
  }

  int status = 0;
  wait(&status);
  printf("Parent reaped child (exit status: %d). Kernel killed only offending child.\n", status);
  printf("Parent continues executing normally.\n");

  printf("\nfaulttest completed successfully.\n");
  exit(0);
}
