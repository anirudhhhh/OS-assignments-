# OS Lab Assignment 5: Process Synchronization

This assignment implements process synchronization problems in xv6: Peterson's Algorithm, Producer-Consumer (Bounded Buffer), and Readers-Writers. (Question 4 is omitted).

## Files Changed/Added

- `kernel/memlayout.h`: Added `SHM_BASE` address for shared memory.
- `kernel/syscall.h`, `kernel/syscall.c`: Added syscall numbers and dispatch entries for `shm_get`, `sem_init`, `sem_wait`, `sem_post`.
- `kernel/defs.h`: Added function prototypes for `shminit` and `seminit`.
- `kernel/main.c`: Initialized shared memory and semaphores during boot.
- `kernel/proc.c`: Copied shared page mappings in `fork()` and unmapped them in `proc_freepagetable()`.
- `kernel/sysproc.c`: Implemented `shm_get` syscall and semaphore syscalls (`sem_init`, `sem_wait`, `sem_post`).
- `user/user.h`, `user/usys.pl`: Added user-level syscall declarations and stubs.
- `user/peterson.c`: Peterson's algorithm user program.
- `user/prodcons.c`: Producer-consumer user program.
- `user/readwrite.c`: Readers-writers user program.
- `Makefile`: Added the new user programs to `UPROGS`.
- `output_logs/`: Contains terminal logs and screenshots for each question.

## How to Build and Run

1. Build and start xv6 in QEMU:
   ```bash
   make clean
   make qemu
   ```

2. Run the test programs inside the xv6 shell:
   - **Question 1**:
     ```
     $ peterson
     ```
   - **Question 2**:
     ```
     $ prodcons
     ```
     *(Optionally pass a buffer size: `$ prodcons 3`)*
   - **Question 3**:
     ```
     $ readwrite
     ```

3. To exit QEMU:
   Press `Ctrl-A` then `X`.
