#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Clone creates a new thread
uint64
sys_clone(void)
{
  uint64 fcn;
  uint64 arg;
  uint64 stack;

  argaddr(0, &fcn);
  argaddr(1, &arg);
  argaddr(2, &stack);

  return clone((void (*)(void*))fcn, (void*)arg, (void*)stack);
}

// Join waits for child thread to finish
uint64
sys_join(void)
{
  return join();
}

#define MAX_SEM 10

struct semaphore {
  int value;
  int active;
  struct spinlock lk;
};

struct semaphore semaphores[MAX_SEM];
struct spinlock sem_table_lock;

// Initialize kernel semaphore table
void
seminit(void)
{
  initlock(&sem_table_lock, "sem_table");
  for (int i = 0; i < MAX_SEM; i++) {
    initlock(&semaphores[i].lk, "semaphore");
    semaphores[i].value = 0;
    semaphores[i].active = 0;
  }
}

// Initialize a specific semaphore
int
ksem_init(int sem_id, int value)
{
  if (sem_id < 0 || sem_id >= MAX_SEM || value < 0)
    return -1;
  struct semaphore *s = &semaphores[sem_id];
  acquire(&s->lk);
  s->value = value;
  s->active = 1;
  release(&s->lk);
  return 0;
}

// Wait on a semaphore (P operation)
int
ksem_wait(int sem_id)
{
  if (sem_id < 0 || sem_id >= MAX_SEM)
    return -1;
  struct semaphore *s = &semaphores[sem_id];
  acquire(&s->lk);
  if (!s->active) {
    release(&s->lk);
    return -1;
  }
  while (s->value <= 0) {
    sleep_prepare(s);
    release(&s->lk);
    sleep();
    acquire(&s->lk);
  }
  s->value--;
  release(&s->lk);
  return 0;
}

// Signal a semaphore (V operation)
int
ksem_signal(int sem_id)
{
  if (sem_id < 0 || sem_id >= MAX_SEM)
    return -1;
  struct semaphore *s = &semaphores[sem_id];
  acquire(&s->lk);
  if (!s->active) {
    release(&s->lk);
    return -1;
  }
  s->value++;
  wakeup(s);
  release(&s->lk);
  return 0;
}

uint64
sys_sem_init(void)
{
  int sem_id, value;
  argint(0, &sem_id);
  argint(1, &value);
  return ksem_init(sem_id, value);
}

uint64
sys_sem_wait(void)
{
  int sem_id;
  argint(0, &sem_id);
  return ksem_wait(sem_id);
}

uint64
sys_sem_signal(void)
{
  int sem_id;
  argint(0, &sem_id);
  return ksem_signal(sem_id);
}

// Set priority of process
uint64
sys_set_priority(void)
{
  int pid, priority;
  argint(0, &pid);
  argint(1, &priority);
  return set_priority(pid, priority);
}

// Get priority of process
uint64
sys_get_priority(void)
{
  int pid;
  argint(0, &pid);
  return get_priority(pid);
}
