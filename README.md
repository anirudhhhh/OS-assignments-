# OS Lab Assignment 5: Process Synchronization

This assignment implements process synchronization problems in xv6: Peterson's Algorithm, Producer-Consumer (Bounded Buffer), and Readers-Writers. (Question 4 is omitted).

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
