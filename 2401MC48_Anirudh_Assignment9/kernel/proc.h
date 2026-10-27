// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;      // The process running on this cpu, or null.
  struct context context; // swtch() here to enter scheduler().
  int noff;               // Depth of push_off() nesting.
  int intena;             // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// prepare_return() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
struct trapframe {
  uint64 kernel_satp; // offset 0   // kernel page table
  uint64 kernel_sp; // offset 8     // top of process's kernel stack
  uint64 kernel_trap; // offset 16   // usertrap()
  uint64 epc; // offset 24           // saved user program counter
  uint64 kernel_hartid; // offset 32 // saved kernel tp
  uint64 ra; // offset 40
  uint64 sp; // offset 48
  uint64 gp; // offset 56
  uint64 tp; // offset 64
  uint64 t0; // offset 72
  uint64 t1; // offset 80
  uint64 t2; // offset 88
  uint64 s0; // offset 96
  uint64 s1; // offset 104
  uint64 a0; // offset 112
  uint64 a1; // offset 120
  uint64 a2; // offset 128
  uint64 a3; // offset 136
  uint64 a4; // offset 144
  uint64 a5; // offset 152
  uint64 a6; // offset 160
  uint64 a7; // offset 168
  uint64 s2; // offset 176
  uint64 s3; // offset 184
  uint64 s4; // offset 192
  uint64 s5; // offset 200
  uint64 s6; // offset 208
  uint64 s7; // offset 216
  uint64 s8; // offset 224
  uint64 s9; // offset 232
  uint64 s10; // offset 240
  uint64 s11; // offset 248
  uint64 t3; // offset 256
  uint64 t4; // offset 264
  uint64 t5; // offset 272
  uint64 t6; // offset 280
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock;

  // p->lock must be held when using these:
  enum procstate state; // Process state
  void *chan;           // If non-zero, sleeping on chan
  int killed;           // If non-zero, have been killed
  int xstate;           // Exit status to be returned to parent's wait
  int pid;              // Process ID

  // wait_lock must be held when using this:
  struct proc *parent; // Parent process

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack
  int faults; // Assignment 9: successful user page faults; reset in allocproc.
  uint64 sz;                   // Size of process memory (bytes)
  pagetable_t pagetable;       // User page table
  struct trapframe *trapframe; // data page for trampoline.S
  struct context context;      // swtch() here to run process
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
};
