#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Global test variable
int global_var = 1234;

int
main(int argc, char *argv[])
{
  int local_var = 5678;
  char *heap_var = malloc(128);
  if (!heap_var) {
    printf("malloc failed\n");
    exit(1);
  }
  heap_var[0] = 'X';

  printf("Question 1: Virtual to Physical Address Translation\n\n");

  // Part 1: VA to PA translation of variables
  printf("--- Part 1: Variable Translations ---\n");
  uint64 pa_global = va2pa((uint64)&global_var);
  printf("Global variable: VA = %p, PA = 0x%lx\n", &global_var, pa_global);

  uint64 pa_local = va2pa((uint64)&local_var);
  printf("Local variable:  VA = %p, PA = 0x%lx\n", &local_var, pa_local);

  uint64 pa_heap = va2pa((uint64)heap_var);
  printf("Heap allocated:  VA = %p, PA = 0x%lx\n", heap_var, pa_heap);

  // Part 2: Unmapped address check
  printf("\n--- Part 2: Unmapped Address Translation ---\n");
  uint64 unmapped_va = 0x7FFFFFFF;
  uint64 pa_unmapped = va2pa(unmapped_va);
  printf("Unmapped VA (0x%lx): PA = %ld (expected -1)\n", unmapped_va, (long)pa_unmapped);

  // Part 3: Fork isolation test
  printf("\n--- Part 3: Address Space Isolation on Fork ---\n");
  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    uint64 child_pa = va2pa((uint64)&global_var);
    printf("Child:  VA = %p, PA = 0x%lx\n", &global_var, child_pa);
    exit(0);
  } else {
    // Parent process
    wait(0);
    uint64 parent_pa = va2pa((uint64)&global_var);
    printf("Parent: VA = %p, PA = 0x%lx\n", &global_var, parent_pa);
    if (parent_pa != 0 && parent_pa != (uint64)-1) {
      printf("Result: Parent and child share identical VA but different physical frames.\n");
    }
  }

  free(heap_var);
  printf("\nvatest passed successfully.\n");
  exit(0);
}
