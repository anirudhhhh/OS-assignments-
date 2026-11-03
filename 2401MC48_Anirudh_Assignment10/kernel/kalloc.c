// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  int free_pages;
  int total_pages;
  int total_allocated;
  int total_freed;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  kmem.total_pages = ((uint64)PHYSTOP - PGROUNDUP((uint64)end)) / PGSIZE;
  kmem.free_pages = 0;
  kmem.total_allocated = 0;
  kmem.total_freed = 0;
  freerange(end, (void *)PHYSTOP);
  // Reset counters after initial free pool creation
  acquire(&kmem.lock);
  kmem.total_allocated = 0;
  kmem.total_freed = 0;
  release(&kmem.lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.free_pages++;
  kmem.total_freed++;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r) {
    kmem.freelist = r->next;
    kmem.free_pages--;
    kmem.total_allocated++;
  }
  release(&kmem.lock);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return (void *)r;
}

// Return physical memory statistics
void
kmem_get_stats(int *allocated, int *freed, int *current, int *available)
{
  acquire(&kmem.lock);
  if (allocated)
    *allocated = kmem.total_allocated;
  if (freed)
    *freed = kmem.total_freed;
  if (current)
    *current = kmem.total_allocated - kmem.total_freed;
  if (available)
    *available = kmem.free_pages;
  release(&kmem.lock);
}

// Return the current number of free physical pages
int
kfree_pages_count(void)
{
  acquire(&kmem.lock);
  int n = kmem.free_pages;
  release(&kmem.lock);
  return n;
}

// Return the total number of physical pages managed
int
ktotal_pages_count(void)
{
  return kmem.total_pages;
}

