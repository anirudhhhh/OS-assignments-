#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Semaphore identifiers
#define SEM_MUTEX 0
#define SEM_WRT 1
#define SEM_ORDER 2
#define SEM_TRACK 3
#define SEM_PRINT 4

// Shared memory structure for readers-writers
struct rw_data {
  volatile int shared_data;
  volatile int read_count;
  volatile int active_readers;
  volatile int active_writers;
};

// Reader process routine
void
reader(int id, struct rw_data *shm)
{
  for (int i = 0; i < 3; i++) {
    // Pass through turnstile
    sem_wait(SEM_ORDER);
    sem_post(SEM_ORDER);

    // Reader entry section
    sem_wait(SEM_MUTEX);
    shm->read_count++;
    if (shm->read_count == 1) {
      sem_wait(SEM_WRT);
    }
    sem_post(SEM_MUTEX);

    // Update active reader counter
    sem_wait(SEM_TRACK);
    shm->active_readers++;
    int cur_r = shm->active_readers;
    int cur_w = shm->active_writers;
    sem_post(SEM_TRACK);

    // Read shared data
    int val = shm->shared_data;
    sem_wait(SEM_PRINT);
    printf("[%d] Reader %d [PID %d]: read %d (readers: %d, writers: %d)\n",
           uptime(), id, getpid(), val, cur_r, cur_w);
    sem_post(SEM_PRINT);

    // Reading delay
    pause(2);

    sem_wait(SEM_TRACK);
    shm->active_readers--;
    sem_post(SEM_TRACK);

    // Reader exit section
    sem_wait(SEM_MUTEX);
    shm->read_count--;
    if (shm->read_count == 0) {
      sem_post(SEM_WRT);
    }
    sem_post(SEM_MUTEX);

    pause(1);
  }
  exit(0);
}

// Writer process routine
void
writer(int id, struct rw_data *shm)
{
  for (int i = 0; i < 2; i++) {
    // Acquire turnstile to prevent writer starvation
    sem_wait(SEM_ORDER);
    sem_wait(SEM_WRT);

    // Update active writer counter
    sem_wait(SEM_TRACK);
    shm->active_writers++;
    int cur_r = shm->active_readers;
    int cur_w = shm->active_writers;
    sem_post(SEM_TRACK);

    // Exclusive write
    shm->shared_data += 10;
    sem_wait(SEM_PRINT);
    printf("[%d] Writer %d [PID %d]: write %d (readers: %d, writers: %d)\n",
           uptime(), id, getpid(), shm->shared_data, cur_r, cur_w);
    sem_post(SEM_PRINT);

    // Writing delay
    pause(2);

    sem_wait(SEM_TRACK);
    shm->active_writers--;
    sem_post(SEM_TRACK);

    // Release writer lock and turnstile
    sem_post(SEM_WRT);
    sem_post(SEM_ORDER);

    pause(1);
  }
  exit(0);
}

int
main(void)
{
  // Attach shared memory
  struct rw_data *shm = (struct rw_data *)shm_get();
  if (shm == 0) {
    printf("readwrite: shm_get failed\n");
    exit(1);
  }

  // Initialize shared variables
  shm->shared_data = 0;
  shm->read_count = 0;
  shm->active_readers = 0;
  shm->active_writers = 0;

  // Initialize semaphores
  sem_init(SEM_MUTEX, 1);
  sem_init(SEM_WRT, 1);
  sem_init(SEM_ORDER, 1);
  sem_init(SEM_TRACK, 1);
  sem_init(SEM_PRINT, 1);

  // Spawn 3 readers
  for (int i = 1; i <= 3; i++) {
    int pid = fork();
    if (pid == 0) {
      reader(i, shm);
    }
  }

  // Spawn 2 writers
  for (int i = 1; i <= 2; i++) {
    int pid = fork();
    if (pid == 0) {
      writer(i, shm);
    }
  }

  // Parent waits for all 5 children
  for (int i = 0; i < 5; i++) {
    wait(0);
  }

  exit(0);
}
