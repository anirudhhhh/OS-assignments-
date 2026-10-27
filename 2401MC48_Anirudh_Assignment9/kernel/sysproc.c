#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "meminfo.h"

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
  int n;

  argint(0, &n);
  addr = myproc()->sz;

  if (n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else if (n > 0) {
    // Lazily allocate memory for this process: increase sz
    // without eagerly allocating physical frames.
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

// Virtual to physical address translation
uint64
sys_va2pa(void)
{
  uint64 va;
  argaddr(0, &va);

  struct proc *p = myproc();
  if (va >= MAXVA)
    return -1;

  pte_t *pte = walk(p->pagetable, va, 0);
  if (pte == 0)
    return -1;
  if ((*pte & PTE_V) == 0)
    return -1;
  if ((*pte & PTE_U) == 0)
    return -1;

  uint64 pa = PTE2PA(*pte) | (va & 0xFFF);

  int pde_idx = PX(2, va);
  int pte1_idx = PX(1, va);
  int pte0_idx = PX(0, va);
  uint offset = va & 0xFFF;
  int P = (*pte & PTE_V) ? 1 : 0;
  int W = (*pte & PTE_W) ? 1 : 0;
  int U = (*pte & PTE_U) ? 1 : 0;

  printk("va2pa: va 0x%lx -> L2:%d L1:%d L0:%d off:0x%x flags:P=%d,W=%d,U=%d -> pa 0x%lx\n",
         va, pde_idx, pte1_idx, pte0_idx, offset, P, W, U, pa);

  return pa;
}

// Physical and process memory statistics
uint64
sys_getmeminfo(void)
{
  uint64 addr;
  argaddr(0, &addr);

  struct proc *p = myproc();
  struct meminfo mi;

  mi.total_pages = ktotal_pages_count();
  mi.free_pages = kfree_pages_count();
  mi.user_pages = p->sz / PGSIZE;
  mi.pagetable_pages = count_pagetable_pages(p->pagetable);

  if (copyout(p->pagetable, p->sz, addr, (char *)&mi, sizeof(mi)) < 0)
    return -1;

  return 0;
}

// Dump process page table
uint64
sys_vmprint(void)
{
  vmprint_process(myproc()->pagetable);
  return 0;
}

// Return number of page faults handled for calling process
uint64
sys_getfaults(void)
{
  return myproc()->faults;
}

