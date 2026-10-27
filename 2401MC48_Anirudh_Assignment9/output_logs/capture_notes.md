# Capture provenance

The .txt files are actual serial output from xv6 RISC-V running in QEMU, captured programmatically. PNG files are browser screenshots of these unchanged transcripts, not screenshots of the macOS Terminal app (which denied computer-use access). The HTML capture views are included for reproducibility. No output or addresses were invented.

The five-fault result is measured relative to getfaults() before the 32-page reservation: the process may already have faults inherited across shell exec. Page-table page counts include root and tables for supervisor trap mappings, while user leaf totals exclude the guard page and trap mappings.
