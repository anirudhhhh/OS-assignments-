// User program to test page table management and page protection
// Question 3: Paging: Page Table Management and Page Protection

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int main(int argc, char *argv[])
{
  printf("=== Question 3: Page Table Management & Page Protection ===\n\n");

  printf("1. Allocating 1 page (4096 bytes) using sbrk()...\n");
  char *page = sbrk(PGSIZE);
  if (page == (char *)-1) {
    printf("pagetest: sbrk failed\n");
    exit(1);
  }

  page[0] = 'H';
  page[1] = 'I';
  printf("Allocated page at VA %p with content: \"%c%c\"\n", page, page[0], page[1]);

  printf("\n2. Inspecting page-table entry before setreadonly():\n");
  pageinfo((uint64)page);

  printf("\n3. Setting page to READ-ONLY via setreadonly()...\n");
  if (setreadonly((uint64)page) < 0) {
    printf("pagetest: setreadonly failed\n");
    exit(1);
  }
  printf("setreadonly() returned success.\n");

  printf("\n4. Inspecting page-table entry after setreadonly():\n");
  pageinfo((uint64)page);

  printf("\n5. Forking child process to verify read and trigger write protection fault...\n");
  int pid = fork();
  if (pid < 0) {
    printf("pagetest: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    printf("[Child pid %d] Reading from read-only page: '%c' (READ SUCCESSFUL)\n", getpid(), page[0]);
    printf("[Child pid %d] Attempting to write 'W' to read-only page at VA %p...\n", getpid(), page);
    page[0] = 'W'; // Should trigger write-protection page fault!
    printf("[Child pid %d] ERROR: Write succeeded on protected page!\n", getpid());
    exit(0);
  } else {
    // Parent process
    int status = 0;
    wait(&status);
    printf("\n[Parent] Child process terminated with status %d\n", status);
    printf("[Parent] Write-protection page fault verified successfully!\n");
  }

  printf("\npagetest completed successfully.\n");
  exit(0);
}
