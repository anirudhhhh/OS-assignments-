#include "kernel/types.h"
#include "user/user.h"

// Assignment 9: exercise sparse clone/free, zero fill and lazy syscall buffers.
int
main(void)
{
  char *p = sbrk(32 * 4096);
  if (p == (char *)-1) exit(1);
  if (p[0] != 0 || p[31 * 4096] != 0) exit(2);
  p[0] = 'A';
  int fd[2];
  if (pipe(fd) < 0) exit(3);
  if (write(fd[1], "Z", 1) != 1 || read(fd[0], p + 10 * 4096, 1) != 1 || p[10 * 4096] != 'Z') exit(4);
  close(fd[0]); close(fd[1]);
  int pid = fork(), status;
  if (pid < 0) exit(5);
  if (pid == 0) {
    if (p[0] != 'A' || p[10 * 4096] != 'Z' || p[20 * 4096] != 0) exit(6);
    p[0] = 'B';
    exit(0);
  }
  if (wait(&status) != pid || status != 0 || p[0] != 'A') exit(7);
  if (sbrk(-32 * 4096) == (char *)-1) exit(8);
  printf("lazycheck: zero fill, syscall buffer, sparse fork, isolation and shrink PASS\n");
  exit(0);
}
