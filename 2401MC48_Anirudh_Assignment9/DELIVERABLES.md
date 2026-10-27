# Assignment 9 deliverables

- Modified kernel sources: kernel/vm.c, trap.c, proc.h, proc.c, sysproc.c, syscall.c, syscall.h, defs.h; supporting kalloc.c and meminfo.h retained from the existing base.
- User system-call interface: user/user.h, usys.pl and generated usys.S; user/ulib.c.
- Required programs: user/pttest.c, user/faulttest.c, user/pagerepl.c.
- Additional verification: user/lazycheck.c; user/sh.c accepts the full input command.
- Build configuration: Makefile.
- Output screenshots: output_logs/q1_pttest.png, q2_faulttest.png, q3_pagerepl.png. These are browser captures of genuine QEMU serial output. The user-supplied native screenshots could be viewed in chat but could not be read from macOS protected screenshot folders.
- Raw output and build evidence: output_logs/*.txt.
- Report: report.pdf, with a one-page explanation and three screenshot appendix pages. The comparison table is omitted from the report at the user’s request; the program and raw output retain it.
- Build and execution instructions: README.md.

Full upstream usertests did not complete; the assignment tests and focused lazy-memory checks passed.
