#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Print hit ratio as percentage (e.g. 25.00%)
static void
print_ratio(int hits, int total)
{
  int ratio_x100 = (hits * 10000) / total;
  int int_part = ratio_x100 / 100;
  int frac_part = ratio_x100 % 100;
  if (frac_part < 10)
    printf("%d.0%d%%", int_part, frac_part);
  else
    printf("%d.%d%%", int_part, frac_part);
}

// Simulate FIFO page replacement
static int
run_fifo(int *refs, int n, int num_frames, int verbose)
{
  int frames[10];
  int count = 0;
  int faults = 0;
  int hits = 0;

  for (int i = 0; i < num_frames; i++)
    frames[i] = -1;

  if (verbose) {
    printf("--- FIFO Page Replacement (Frames: %d) ---\n", num_frames);
    printf("Ref | Frames         | Status\n");
    printf("----+----------------+-------\n");
  }

  for (int i = 0; i < n; i++) {
    int page = refs[i];
    int hit_idx = -1;
    for (int j = 0; j < count; j++) {
      if (frames[j] == page) {
        hit_idx = j;
        break;
      }
    }

    if (hit_idx != -1) {
      hits++;
      if (verbose) {
        printf(" %d  | [ ", page);
        for (int j = 0; j < num_frames; j++) {
          if (j < count)
            printf("%d ", frames[j]);
          else
            printf("- ");
        }
        printf("]     | HIT\n");
      }
    } else {
      faults++;
      if (count < num_frames) {
        frames[count++] = page;
      } else {
        // Shift queue left and append new page
        for (int j = 0; j < num_frames - 1; j++) {
          frames[j] = frames[j + 1];
        }
        frames[num_frames - 1] = page;
      }
      if (verbose) {
        printf(" %d  | [ ", page);
        for (int j = 0; j < num_frames; j++) {
          if (j < count)
            printf("%d ", frames[j]);
          else
            printf("- ");
        }
        printf("]     | FAULT\n");
      }
    }
  }

  if (verbose) {
    printf("FIFO Summary: %d Faults, %d Hits, Hit Ratio: ", faults, hits);
    print_ratio(hits, n);
    printf("\n\n");
  }

  return faults;
}

// Simulate LRU page replacement
static int
run_lru(int *refs, int n, int num_frames, int verbose)
{
  int frames[10];
  int count = 0;
  int faults = 0;
  int hits = 0;

  for (int i = 0; i < num_frames; i++)
    frames[i] = -1;

  if (verbose) {
    printf("--- LRU Page Replacement (Frames: %d) ---\n", num_frames);
    printf("Ref | Frames         | Status\n");
    printf("----+----------------+-------\n");
  }

  for (int i = 0; i < n; i++) {
    int page = refs[i];
    int hit_idx = -1;
    for (int j = 0; j < count; j++) {
      if (frames[j] == page) {
        hit_idx = j;
        break;
      }
    }

    if (hit_idx != -1) {
      hits++;
      // Move accessed page to end (most recently used)
      int accessed = frames[hit_idx];
      for (int j = hit_idx; j < count - 1; j++) {
        frames[j] = frames[j + 1];
      }
      frames[count - 1] = accessed;

      if (verbose) {
        printf(" %d  | [ ", page);
        for (int j = 0; j < num_frames; j++) {
          if (j < count)
            printf("%d ", frames[j]);
          else
            printf("- ");
        }
        printf("]     | HIT\n");
      }
    } else {
      faults++;
      if (count < num_frames) {
        frames[count++] = page;
      } else {
        // Evict least recently used (index 0), shift left, append
        for (int j = 0; j < num_frames - 1; j++) {
          frames[j] = frames[j + 1];
        }
        frames[num_frames - 1] = page;
      }
      if (verbose) {
        printf(" %d  | [ ", page);
        for (int j = 0; j < num_frames; j++) {
          if (j < count)
            printf("%d ", frames[j]);
          else
            printf("- ");
        }
        printf("]     | FAULT\n");
      }
    }
  }

  if (verbose) {
    printf("LRU Summary: %d Faults, %d Hits, Hit Ratio: ", faults, hits);
    print_ratio(hits, n);
    printf("\n\n");
  }

  return faults;
}

int
main(int argc, char *argv[])
{
  int default_refs[20] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
  int *refs = default_refs;
  int n = 20;

  printf("Question 3: Page Replacement Algorithms (FIFO and LRU)\n\n");

  if (argc == 2) {
    fprintf(2, "usage: pagerepl [frames page ...]\n");
    exit(1);
  }
  if (argc >= 3) {
    // Custom input from command line
    int custom_frames = atoi(argv[1]);
    if (custom_frames < 1 || custom_frames > 10) {
      fprintf(2, "pagerepl: frames must be between 1 and 10\n");
      exit(1);
    }
    for (int i = 1; i < argc; i++) {
      if (!argv[i][0]) exit(1);
      int value = 0;
      for (char *c = argv[i]; *c; c++) {
        if (*c < '0' || *c > '9' || value > (2147483647 - (*c - '0')) / 10) {
          fprintf(2, "pagerepl: use nonnegative integer input\n");
          exit(1);
        }
        value = value * 10 + *c - '0';
      }
    }
    int custom_refs[50];
    int custom_n = argc - 2;
    if (custom_n > 50)
      custom_n = 50;
    for (int i = 0; i < custom_n; i++) {
      custom_refs[i] = atoi(argv[i + 2]);
    }
    printf("Reference string length: %d, Frames: %d\n\n", custom_n, custom_frames);
    run_fifo(custom_refs, custom_n, custom_frames, 1);
    run_lru(custom_refs, custom_n, custom_frames, 1);
    exit(0);
  }

  printf("Reference string: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1 (length = %d)\n\n", n);

  // 1. Detailed run for 3 frames
  run_fifo(refs, n, 3, 1);
  run_lru(refs, n, 3, 1);

  // 2. Comparative table for 3, 4, and 5 frames
  printf("Comparative Performance Table (3, 4, and 5 Frames)\n");
  printf("Frames | Algorithm | Page Faults | Page Hits | Hit Ratio\n");
  printf("-------+-----------+-------------+-----------+----------\n");

  int frame_tests[3] = {3, 4, 5};
  for (int i = 0; i < 3; i++) {
    int f = frame_tests[i];

    int fifo_faults = run_fifo(refs, n, f, 0);
    int fifo_hits = n - fifo_faults;
    printf("   %d   | FIFO      |      %d     |     %d    |   ", f, fifo_faults, fifo_hits);
    print_ratio(fifo_hits, n);
    printf("\n");

    int lru_faults = run_lru(refs, n, f, 0);
    int lru_hits = n - lru_faults;
    printf("   %d   | LRU       |      %d     |     %d    |   ", f, lru_faults, lru_hits);
    print_ratio(lru_hits, n);
    printf("\n");

    if (i < 2)
      printf("-------+-----------+-------------+-----------+----------\n");
  }

  printf("\npagerepl completed successfully.\n");
  exit(0);
}
