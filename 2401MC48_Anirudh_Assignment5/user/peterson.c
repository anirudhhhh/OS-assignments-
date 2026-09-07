#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Shared memory layout for Peterson's algorithm
struct peterson_data {
  volatile int flag[2];
  volatile int turn;
  volatile int shared_counter;
};

// Remainder section delay
void
delay(void)
{
  for (volatile int d = 0; d < 200000; d++) {
  }
}

int
main(void)
{
  // Attach shared memory
  struct peterson_data *shm = (struct peterson_data *)shm_get();
  if (shm == 0) {
    printf("peterson: shm_get failed\n");
    exit(1);
  }

  // Initialize shared variables
  shm->flag[0] = 0;
  shm->flag[1] = 0;
  shm->turn = 0;
  shm->shared_counter = 0;

  int pid = fork();
  if (pid < 0) {
    printf("peterson: fork failed\n");
    exit(1);
  }

  // Process 0 is parent, process 1 is child
  int id = (pid == 0) ? 1 : 0;
  int other = 1 - id;

  for (int iter = 0; iter < 10; iter++) {
    // Entry section
    shm->flag[id] = 1;
    __sync_synchronize();
    shm->turn = other;
    __sync_synchronize();

    while (shm->flag[other] == 1 && shm->turn == other) {
      // Busy wait
    }

    // Critical section
    shm->shared_counter++;
    printf("Process %d in CS, counter = %d\n", id, shm->shared_counter);
    printf("Process %d exiting CS\n", id);

    // Exit section
    __sync_synchronize();
    shm->flag[id] = 0;

    // Remainder section
    delay();
  }

  if (id == 1) {
    exit(0);
  }

  wait(0);
  printf("Final counter = %d\n", shm->shared_counter);
  exit(0);
}
