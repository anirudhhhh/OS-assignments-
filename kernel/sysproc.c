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

// Shared memory structure
struct {
  struct spinlock lock;
  char *pages[16];
} shm_table;

// Initialize shared memory table
void
shminit(void)
{
  initlock(&shm_table.lock, "shm_table");
  for (int i = 0; i < 16; i++) {
    shm_table.pages[i] = 0;
  }
}

// System call to allocate/attach shared memory page
uint64
sys_shm_get(void)
{
  int id = 0;
  argint(0, &id);
  if (id < 0 || id >= 16) {
    id = 0;
  }

  struct proc *p = myproc();
  uint64 va = SHM_BASE + id * PGSIZE;

  // If already mapped in process, return virtual address
  if (ismapped(p->pagetable, va)) {
    return va;
  }

  acquire(&shm_table.lock);
  if (shm_table.pages[id] == 0) {
    shm_table.pages[id] = kalloc();
    if (shm_table.pages[id] == 0) {
      release(&shm_table.lock);
      return 0;
    }
    memset(shm_table.pages[id], 0, PGSIZE);
  }

  if (mappages(p->pagetable, va, PGSIZE, (uint64)shm_table.pages[id], PTE_R | PTE_W | PTE_U) != 0) {
    release(&shm_table.lock);
    return 0;
  }
  release(&shm_table.lock);

  return va;
}

// Maximum semaphores supported
#define MAX_SEMS 32

// Semaphore structure
struct sem {
  int value;
  int active;
  struct spinlock lock;
};

// Semaphore table
struct {
  struct spinlock lock;
  struct sem sems[MAX_SEMS];
} sem_table;

// Initialize semaphore table
void
seminit(void)
{
  initlock(&sem_table.lock, "sem_table");
  for (int i = 0; i < MAX_SEMS; i++) {
    initlock(&sem_table.sems[i].lock, "sem");
    sem_table.sems[i].active = 0;
    sem_table.sems[i].value = 0;
  }
}

// System call to initialize a semaphore
uint64
sys_sem_init(void)
{
  int id, val;
  argint(0, &id);
  argint(1, &val);

  if (id < 0 || id >= MAX_SEMS) {
    return -1;
  }

  acquire(&sem_table.sems[id].lock);
  sem_table.sems[id].value = val;
  sem_table.sems[id].active = 1;
  release(&sem_table.sems[id].lock);
  return 0;
}

// System call to wait on a semaphore
uint64
sys_sem_wait(void)
{
  int id;
  argint(0, &id);

  if (id < 0 || id >= MAX_SEMS) {
    return -1;
  }

  acquire(&sem_table.sems[id].lock);
  while (sem_table.sems[id].value <= 0) {
    sleep_prepare(&sem_table.sems[id]);
    release(&sem_table.sems[id].lock);
    sleep();
    acquire(&sem_table.sems[id].lock);
  }
  sem_table.sems[id].value--;
  release(&sem_table.sems[id].lock);
  return 0;
}

// System call to signal/post a semaphore
uint64
sys_sem_post(void)
{
  int id;
  argint(0, &id);

  if (id < 0 || id >= MAX_SEMS) {
    return -1;
  }

  acquire(&sem_table.sems[id].lock);
  sem_table.sems[id].value++;
  wakeup(&sem_table.sems[id]);
  release(&sem_table.sems[id].lock);
  return 0;
}

