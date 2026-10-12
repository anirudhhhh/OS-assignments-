#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 10

// Part 1: Synchronized counter test
volatile int counter = 0;
volatile int start_flag = 0;

void
sync_worker(void *arg)
{
  while (!start_flag)
    ;
  for (int i = 0; i < 100000; i++) {
    sem_wait(0);
    counter++;
    sem_signal(0);
  }
  exit(0);
}

// Part 2: Producer-Consumer queue
int buffer[BUFFER_SIZE];
int in_idx = 0;
int out_idx = 0;

#define SEM_EMPTY 1
#define SEM_FULL  2
#define SEM_MUTEX 3

void
producer_worker(void *arg)
{
  for (int i = 1; i <= TOTAL_ITEMS; i++) {
    sem_wait(SEM_EMPTY);
    sem_wait(SEM_MUTEX);

    buffer[in_idx] = i;
    printf("Producer produced item %d at buffer index %d\n", i, in_idx);
    in_idx = (in_idx + 1) % BUFFER_SIZE;

    sem_signal(SEM_MUTEX);
    sem_signal(SEM_FULL);
  }
  exit(0);
}

void
consumer_worker(void *arg)
{
  for (int i = 1; i <= TOTAL_ITEMS; i++) {
    sem_wait(SEM_FULL);
    sem_wait(SEM_MUTEX);

    int item = buffer[out_idx];
    printf("Consumer consumed item %d from buffer index %d\n", item, out_idx);
    out_idx = (out_idx + 1) % BUFFER_SIZE;

    sem_signal(SEM_MUTEX);
    sem_signal(SEM_EMPTY);
  }
  exit(0);
}

int
main(int argc, char *argv[])
{
  printf("Part 1: Synchronized Counter Test with Semaphores\n");
  sem_init(0, 1);
  counter = 0;

  for (int i = 0; i < 3; i++) {
    void *stack = malloc(4096);
    if (!stack) {
      printf("Failed to allocate stack\n");
      exit(1);
    }
    int tid = clone(sync_worker, (void *)(uint64)i, stack);
    if (tid < 0) {
      printf("clone failed\n");
      exit(1);
    }
    printf("Created sync thread PID %d\n", tid);
  }

  start_flag = 1;

  for (int i = 0; i < 3; i++) {
    int tid = join();
    printf("Sync thread %d joined\n", tid);
  }

  printf("Final counter value: %d (expected exactly 300000)\n\n", counter);

  printf("Part 2: Producer-Consumer Simulation (Buffer size 5)\n");
  sem_init(SEM_EMPTY, BUFFER_SIZE);
  sem_init(SEM_FULL, 0);
  sem_init(SEM_MUTEX, 1);

  void *p_stack = malloc(4096);
  void *c_stack = malloc(4096);
  if (!p_stack || !c_stack) {
    printf("Failed to allocate stack\n");
    exit(1);
  }

  int prod_pid = clone(producer_worker, 0, p_stack);
  int cons_pid = clone(consumer_worker, 0, c_stack);

  printf("Started producer (PID %d) and consumer (PID %d)\n", prod_pid, cons_pid);

  join();
  join();

  printf("Producer-Consumer simulation completed successfully.\n");
  exit(0);
}
