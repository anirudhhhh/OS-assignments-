# OS Lab Assignment 7: Threads, Semaphores, Deadlock Detection, and Priority Scheduling

This assignment implements kernel-supported threads (`clone` and `join`), counting semaphores with Producer-Consumer synchronization, user-space Deadlock Detection and Banker's Algorithm avoidance, and Priority-Based Preemptive CPU Scheduling with aging in xv6.

## How to Build and Run

1. Build and start xv6 in QEMU:
   ```bash
   make clean
   make qemu
   ```

2. Run the test programs inside the xv6 shell:
   - **Question 1 (User-Level Threads & Race Condition)**:
     ```
     $ threadtest
     ```
   - **Question 2 (Semaphore Synchronization & Producer-Consumer)**:
     ```
     $ synctest
     ```
   - **Question 3 (Deadlock Detection & Avoidance)**:
     ```
     $ deadlock
     ```
   - **Question 4 (Priority Preemptive CPU Scheduling with Aging)**:
     ```
     $ priotest
     ```

3. To exit QEMU:
   Press `Ctrl-A` then `X`.
