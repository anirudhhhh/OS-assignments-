# OS Lab Assignment 6: Deadlock Avoidance and Detection

This assignment implements Banker's Algorithm simulation, Resource Allocation Graph cycle detection, Deadlock Prevention via Resource Ordering, and Combined Multi-Resource Synchronization with Deadlock Avoidance in xv6.

## How to Build and Run

1. Build and start xv6 in QEMU:
   ```bash
   make clean
   make qemu
   ```

2. Run the test programs inside the xv6 shell:
   - **Question 1 (Banker's Algorithm)**:
     ```
     $ bankers
     ```
   - **Question 2 (Deadlock Detection)**:
     ```
     $ deadlockdetect
     ```
   - **Question 3 (Resource Ordering)**:
     ```
     $ resourceorder bad
     $ resourceorder fixed
     ```
   - **Question 4 (Combined Synchronization & Deadlock Avoidance)**:
     ```
     $ syncdeadlock
     ```

3. To exit QEMU:
   Press `Ctrl-A` then `X`.
