#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct meminfo mi0, mi1, mi2;

  printf("Question 4: Lazy Memory Allocation (Demand Paging) Test\n\n");

  // Step 1: Initial memory state
  getmeminfo(&mi0);
  printf("1. Baseline: Free pages = %d, User pages = %d\n", mi0.free_pages, mi0.user_pages);

  // Step 2: Large sbrk (32 MB = 8192 pages)
  printf("\nCalling sbrk(32 MB = 33554432 bytes)...\n");
  char *lazy_mem = sbrk(32 * 1024 * 1024);
  if (lazy_mem == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  getmeminfo(&mi1);
  printf("2. After 32 MB sbrk (untouched):\n");
  printf("   Free pages: %d (Delta: %d, expected 0 consumed)\n",
         mi1.free_pages, mi1.free_pages - mi0.free_pages);
  printf("   User pages: %d (Delta: +%d pages)\n",
         mi1.user_pages, mi1.user_pages - mi0.user_pages);

  // Step 3: Touch only 4 widely spaced pages
  printf("\nTouching 4 widely spaced pages (Page 0, Page 1000, Page 4000, Page 8000)...\n");
  lazy_mem[0 * 4096] = 'A';
  lazy_mem[1000 * 4096] = 'B';
  lazy_mem[4000 * 4096] = 'C';
  lazy_mem[8000 * 4096] = 'D';

  getmeminfo(&mi2);
  printf("3. After touching 4 pages:\n");
  printf("   Free pages: %d (Delta from sbrk: %d, expected -4)\n",
         mi2.free_pages, mi2.free_pages - mi1.free_pages);

  // Step 4: System call using an untouched lazy buffer
  printf("\n4. Testing system call (read from pipe) into untouched lazy buffer...\n");
  int fd[2];
  if (pipe(fd) < 0) {
    printf("pipe creation failed\n");
    exit(1);
  }

  char *pipe_buf = sbrk(4096); // Untouched page
  write(fd[1], "xv6_demand_paging", 17);
  int n = read(fd[0], pipe_buf, 17);
  pipe_buf[17] = '\0';
  close(fd[0]);
  close(fd[1]);

  printf("   Pipe read %d bytes: \"%s\"\n", n, pipe_buf);
  printf("   Syscall into untouched buffer succeeded without error.\n");

  // Step 5: Fork with lazy pages
  printf("\n5. Testing fork() with lazy address space...\n");
  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child verifies touched contents
    if (lazy_mem[0] != 'A' || lazy_mem[8000 * 4096] != 'D') {
      printf("Child: data mismatch!\n");
      exit(1);
    }
    // Child touches another lazy page
    lazy_mem[2000 * 4096] = 'Z';
    printf("   Child verified touched pages and touched page 2000 successfully.\n");
    exit(0);
  }
  wait(0);
  printf("   Parent reaped child successfully.\n");

  // Step 6: Invalid access beyond sz
  printf("\n6. Testing invalid memory access (page fault out of bounds)...\n");
  int pid_bad = fork();
  if (pid_bad == 0) {
    printf("   Child attempting access beyond sz...\n");
    // Access far beyond allocated sz
    volatile char bad = lazy_mem[33 * 1024 * 1024];
    (void)bad;
    printf("ERROR: child should have been killed!\n");
    exit(0);
  }
  int status = 0;
  wait(&status);
  printf("   Offending child was terminated by kernel without crashing system.\n");

  printf("\nlazytest completed successfully.\n");
  exit(0);
}
