# Assignment 9 implementation plan

Use the installed xv6 RISC-V tree as the base and build in an isolated copy.
1. Q1: walk Sv39 leaf mappings with PTE_U, count all page-table frames, and verify four touched heap pages.
2. Q2: retain lazy sbrk, sparse unmap/copy, initialize per-process fault counters, and verify five faults and child-only termination.
3. Q3: validate simulator input and verify FIFO/LRU traces and the 3/4/5-frame comparison.
4. Build with -Wall -Werror, run in QEMU, save raw logs, capture output screenshots, and produce a one-page report.

Architecture: PTE_V represents x86 P; Sv39 uses three levels. Supervisor trap mappings must not appear as user pages. Lazy sbrk reserves pages; pttest must touch them before vmprint shows four additional mappings.
