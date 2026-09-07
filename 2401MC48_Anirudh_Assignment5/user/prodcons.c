#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Semaphore IDs
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

// Circular buffer structure in shared memory
struct bounded_buffer {
  int buf[32];
  int head;
  int tail;
  int count;
  int capacity;
};

int
main(int argc, char *argv[])
{
  int buf_size = 5;
  if (argc > 1) {
    buf_size = atoi(argv[1]);
    if (buf_size <= 0 || buf_size > 32) {
      buf_size = 5;
    }
  }

  // Attach shared memory
  struct bounded_buffer *shm = (struct bounded_buffer *)shm_get();
  if (shm == 0) {
    printf("prodcons: shm_get failed\n");
    exit(1);
  }

  // Initialize buffer
  shm->head = 0;
  shm->tail = 0;
  shm->count = 0;
  shm->capacity = buf_size;

  // Initialize semaphores
  sem_init(SEM_MUTEX, 1);
  sem_init(SEM_EMPTY, buf_size);
  sem_init(SEM_FULL, 0);

  int pid = fork();
  if (pid < 0) {
    printf("prodcons: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process is consumer
    for (int i = 1; i <= 20; i++) {
      if (shm->count == 0) {
        printf("Consumer [PID %d]: buffer EMPTY (0/%d), blocking\n", getpid(), shm->capacity);
      }

      sem_wait(SEM_FULL);
      sem_wait(SEM_MUTEX);

      int item = shm->buf[shm->head];
      shm->head = (shm->head + 1) % shm->capacity;
      shm->count--;

      printf("Consumer [PID %d]: consumed %d (buffer %d/%d)\n", getpid(), item, shm->count, shm->capacity);

      sem_post(SEM_MUTEX);
      sem_post(SEM_EMPTY);

      // Consumer delay
      pause(3);
    }
    exit(0);
  }

  // Parent process is producer
  for (int item = 1; item <= 20; item++) {
    if (shm->count == shm->capacity) {
      printf("Producer [PID %d]: buffer FULL (%d/%d), blocking\n", getpid(), shm->count, shm->capacity);
    }

    sem_wait(SEM_EMPTY);
    sem_wait(SEM_MUTEX);

    shm->buf[shm->tail] = item;
    shm->tail = (shm->tail + 1) % shm->capacity;
    shm->count++;

    printf("Producer [PID %d]: produced %d (buffer %d/%d)\n", getpid(), item, shm->count, shm->capacity);

    sem_post(SEM_MUTEX);
    sem_post(SEM_FULL);

    // Producer delay
    pause(1);
  }

  wait(0);
  exit(0);
}
