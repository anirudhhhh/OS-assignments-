# OS Lab Assignment 9: Paging in xv6

Print user page tables, handle lazy allocation faults, and compare FIFO and LRU page replacement.

## Build and run

Overlay this folder's kernel/, user/ and Makefile onto the existing xv6 RISC-V source tree. Then run:

```sh
make clean
make qemu
```

Inside xv6:

```
pttest
faulttest
pagerepl
pagerepl 3 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```

Exit QEMU with Ctrl-A, then X.
