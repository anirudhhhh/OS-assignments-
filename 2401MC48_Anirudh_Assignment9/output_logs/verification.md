# Verification

Base: installed xv6-riscv commit 7d01506ee03394b6b1a161ccee69cd150ab471fc, with the existing Assignment 8 extensions retained. Overlay all supplied sources onto this base.

- Clean kernel and filesystem build succeeded with -Wall -Werror (build.txt).
- pttest: user pages 3 -> 7; page-table frames 5 -> 5.
- faulttest: no faults on reservation, five additional handled faults on five touches; invalid store kills child with status -1.
- pagerepl: 3-frame FIFO/LRU faults 15/12; 4-frame 10/8; 5-frame 9/7. The full supplied custom command was also tested.
- Invalid zero-frame input rejected.
- lazycheck: zero fill, kernel copyout into untouched buffer, sparse fork, address-space isolation, and sparse shrink passed.
- No block comments remain in submitted C/header sources.
- Full upstream usertests stalled after startup and was stopped; no full-suite pass is claimed.

Screenshots are browser captures of the actual saved QEMU transcripts; see capture_notes.md.
