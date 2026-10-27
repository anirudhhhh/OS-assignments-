#ifndef _MEMINFO_H_
#define _MEMINFO_H_

// Memory statistics structure for getmeminfo syscall
struct meminfo {
  int total_pages;
  int free_pages;
  int user_pages;
  int pagetable_pages;
};

#endif
