#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

volatile int counter = 0;
volatile int start_flag = 0;

void
worker(void *arg)
{
  while (!start_flag)
    ;
  for (int i = 0; i < 100000; i++) {
    counter++;
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  printf("Starting threadtest with 3 threads...\n");
  for (int i = 0; i < 3; i++) {
    void *stack = malloc(4096);
    if (!stack) {
      printf("Failed to allocate stack\n");
      exit(1);
    }
    int tid = clone(worker, (void *)(uint64)i, stack);
    if (tid < 0) {
      printf("clone failed\n");
      exit(1);
    }
    printf("Created thread with PID %d\n", tid);
  }

  // Release all 3 threads concurrently
  start_flag = 1;

  for (int i = 0; i < 3; i++) {
    int tid = join();
    printf("Thread %d joined\n", tid);
  }

  printf("Final counter value: %d (expected < 300000 due to race condition)\n", counter);
  exit(0);
}
