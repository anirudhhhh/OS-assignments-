#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// CPU-bound worker function
void
cpu_work(int id, int init_prio, int start_tick, uint64 iters)
{
  int my_pid = getpid();
  volatile uint64 sum = 0;
  for (uint64 i = 0; i < iters; i++) {
    sum += (i * 3 + 7);
  }
  (void)sum;
  int finish_tick = uptime();
  int final_prio = get_priority(my_pid);
  sem_wait(0);
  printf("Process P%d (PID %d, Init Prio %d, Final Prio %d) finished at tick %d (Elapsed: %d ticks)\n",
         id, my_pid, init_prio, final_prio, finish_tick, finish_tick - start_tick);
  sem_signal(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  sem_init(0, 1);
  int priorities[4] = {5, 10, 15, 20};
  uint64 work_iters[4] = {
    1300000000ULL, // P0 (prio 5)
    1300000000ULL, // P1 (prio 10)
    1300000000ULL, // P2 (prio 15)
    300000000ULL   // P3 (prio 20)
  };
  int pids[4];

  printf("Question 4: Priority-Based Preemptive Scheduling with Aging Test\n");
  printf("Spawning 4 CPU-bound processes with priorities 5, 10, 15, 20...\n\n");

  int start_tick = uptime();

  for (int i = 0; i < 4; i++) {
    int pid = fork();
    if (pid < 0) {
      printf("fork failed\n");
      exit(1);
    }
    if (pid == 0) {
      cpu_work(i, priorities[i], start_tick, work_iters[i]);
    } else {
      set_priority(pid, priorities[i]);
      pids[i] = pid;
    }
  }

  // Parent monitors P3's priority over time
  for (int m = 0; m < 5; m++) {
    pause(15);
    int p3_prio = get_priority(pids[3]);
    if (p3_prio > 0) {
      printf("[Aging Monitor] Tick %d: Process P3 (PID %d) current priority is %d\n",
             uptime(), pids[3], p3_prio);
    }
  }

  for (int i = 0; i < 4; i++) {
    wait(0);
  }

  printf("\nAll 4 processes finished successfully.\n");
  printf("Priority scheduling with aging verification complete.\n");
  exit(0);
}
