# OS Lab Assignment 10: Memory Management, Virtual Memory, Paging & Page Replacement

**Student Name**: Anirudh  
**Roll Number**: `2401MC48`  
**Environment**: xv6-riscv (Sv39 64-bit Paging, QEMU)

---

## Structure
- `kernel/`: Modified kernel source files (`kalloc.c`, `vm.c`, `sysproc.c`, `syscall.c`, `syscall.h`, `defs.h`, `proc.h`, `proc.c`, `trap.c`).
- `user/`: User-space test programs (`memtest.c`, `vttest.c`, `pagetest.c`, `pagereplace.c`, `user.h`, `usys.pl`, `ulib.c`).
- `output_logs/`: Terminal outputs, execution logs, screenshots, and comparison chart.
- `report.pdf`: Comprehensive 2-page lab report.
- `Makefile`: Build configuration for xv6 with Assignment 10 user programs.

---

## Instructions to Run in xv6
1. Build and run QEMU:
   ```bash
   make clean
   make qemu
   ```
2. Run user test programs in the xv6 shell:
   - `memtest` (Question 1: Physical memory tracking)
   - `vttest` (Question 2: Virtual-to-physical translation)
   - `pagetest` (Question 3: Paging and write protection)
   - `pagereplace` (Question 4: FIFO, LRU, Optimal page replacement)
