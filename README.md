# OS Lab Assignment 9: Page Table Dump, Demand Paging, and Page Replacement Algorithms

This assignment implements process page table dumping (`vmprint`), lazy page allocation with handled page faults accounting (`faulttest`), and FIFO/LRU page replacement algorithms comparison (`pagerepl`) in xv6.

## How to Build and Run

1. Build and start xv6 in QEMU:
   ```bash
   make clean
   make qemu
   ```

2. Run the test programs inside the xv6 shell:
   - **Question 1 (Printing the Page Table of a Process)**:
     ```
     $ pttest
     ```
   - **Question 2 (Handling Page Faults & Lazy Allocation)**:
     ```
     $ faulttest
     ```
   - **Question 3 (Page Replacement Algorithms: FIFO and LRU)**:
     ```
     $ pagerepl
     ```

3. To exit QEMU:
   Press `Ctrl-A` then `X`.
