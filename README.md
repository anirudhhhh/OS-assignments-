# OS Lab Assignment 8: Memory Management in xv6

This assignment implements virtual-to-physical address translation (`va2pa`), kernel and user physical memory statistics tracking (`getmeminfo`), hierarchical page table inspection (`vmprint`), and lazy page allocation / demand paging with copyin/copyout support in xv6.

## How to Build and Run

1. Build and start xv6 in QEMU:
   ```bash
   make clean
   make qemu
   ```

2. Run the test programs inside the xv6 shell:
   - **Question 1 (Virtual to Physical Address Translation)**:
     ```
     $ vatest
     ```
   - **Question 2 (Physical Memory Statistics & Verification)**:
     ```
     $ memtest
     ```
   - **Question 3 (Hierarchical Page Table Dump & Guard Page Verification)**:
     ```
     $ vmprinttest
     ```
   - **Question 4 (Lazy Allocation & Demand Paging)**:
     ```
     $ lazytest
     ```

3. To exit QEMU:
   Press `Ctrl-A` then `X`.
