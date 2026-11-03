// User program to simulate and evaluate Page Replacement Algorithms
// Question 4: FIFO, LRU and Optimal Page Replacement

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_REFS 30
#define MAX_FRAMES 10

static int ref_string[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
static int num_refs = 20;

// Print frames helper
static void print_frames(int frames[], int num_frames)
{
  printf("[");
  for (int i = 0; i < num_frames; i++) {
    if (frames[i] == -1)
      printf(" - ");
    else
      printf(" %d ", frames[i]);
  }
  printf("]");
}

// Helper to print right-aligned integer
static void print_pad(int n, int width)
{
  char buf[16];
  int i = 0;
  if (n == 0) {
    buf[i++] = '0';
  } else {
    int tmp = n;
    while (tmp > 0) {
      buf[i++] = '0' + (tmp % 10);
      tmp /= 10;
    }
  }
  for (int s = 0; s < width - i; s++)
    printf(" ");
  for (int s = i - 1; s >= 0; s--)
    printf("%c", buf[s]);
}

// FIFO Simulation
// Returns total faults
static int sim_fifo(int num_frames, int verbose)
{
  int frames[MAX_FRAMES];
  int next_replace = 0;
  int faults = 0;

  for (int i = 0; i < num_frames; i++)
    frames[i] = -1;

  if (verbose)
    printf("--- FIFO Simulation (%d Frames) ---\n", num_frames);

  for (int i = 0; i < num_refs; i++) {
    int p = ref_string[i];
    int hit = 0;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == p) {
        hit = 1;
        break;
      }
    }

    if (!hit) {
      faults++;
      frames[next_replace] = p;
      next_replace = (next_replace + 1) % num_frames;
      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> FAULT\n");
      }
    } else {
      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> HIT\n");
      }
    }
  }

  if (verbose) {
    int hits = num_refs - faults;
    printf("FIFO Result: %d Faults, %d Hits (Hit Ratio: %d%%)\n\n",
           faults, hits, (hits * 100) / num_refs);
  }
  return faults;
}

// LRU Simulation
// Returns total faults
static int sim_lru(int num_frames, int verbose)
{
  int frames[MAX_FRAMES];
  int last_used[MAX_FRAMES];
  int faults = 0;

  for (int i = 0; i < num_frames; i++) {
    frames[i] = -1;
    last_used[i] = -1;
  }

  if (verbose)
    printf("--- LRU Simulation (%d Frames) ---\n", num_frames);

  for (int i = 0; i < num_refs; i++) {
    int p = ref_string[i];
    int hit_idx = -1;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == p) {
        hit_idx = j;
        break;
      }
    }

    if (hit_idx != -1) {
      last_used[hit_idx] = i;
      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> HIT\n");
      }
    } else {
      faults++;
      int replace_idx = 0;
      int oldest_time = 99999;

      for (int j = 0; j < num_frames; j++) {
        if (frames[j] == -1) {
          replace_idx = j;
          break;
        }
        if (last_used[j] < oldest_time) {
          oldest_time = last_used[j];
          replace_idx = j;
        }
      }

      frames[replace_idx] = p;
      last_used[replace_idx] = i;

      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> FAULT\n");
      }
    }
  }

  if (verbose) {
    int hits = num_refs - faults;
    printf("LRU Result: %d Faults, %d Hits (Hit Ratio: %d%%)\n\n",
           faults, hits, (hits * 100) / num_refs);
  }
  return faults;
}

// Optimal Simulation
// Returns total faults
static int sim_opt(int num_frames, int verbose)
{
  int frames[MAX_FRAMES];
  int faults = 0;

  for (int i = 0; i < num_frames; i++)
    frames[i] = -1;

  if (verbose)
    printf("--- Optimal Simulation (%d Frames) ---\n", num_frames);

  for (int i = 0; i < num_refs; i++) {
    int p = ref_string[i];
    int hit = 0;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == p) {
        hit = 1;
        break;
      }
    }

    if (hit) {
      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> HIT\n");
      }
    } else {
      faults++;
      int replace_idx = -1;
      for (int j = 0; j < num_frames; j++) {
        if (frames[j] == -1) {
          replace_idx = j;
          break;
        }
      }

      if (replace_idx == -1) {
        int farthest_use = -1;
        for (int j = 0; j < num_frames; j++) {
          int next_use = 99999;
          for (int k = i + 1; k < num_refs; k++) {
            if (ref_string[k] == frames[j]) {
              next_use = k;
              break;
            }
          }
          if (next_use > farthest_use) {
            farthest_use = next_use;
            replace_idx = j;
          }
        }
      }

      frames[replace_idx] = p;

      if (verbose) {
        printf("Ref ");
        print_pad(i + 1, 2);
        printf(" (Page %d): ", p);
        print_frames(frames, num_frames);
        printf(" -> FAULT\n");
      }
    }
  }

  if (verbose) {
    int hits = num_refs - faults;
    printf("Optimal Result: %d Faults, %d Hits (Hit Ratio: %d%%)\n\n",
           faults, hits, (hits * 100) / num_refs);
  }
  return faults;
}

static void print_table_row(int fr, const char *algo, int faults)
{
  int hits = num_refs - faults;
  int hit_pct = (hits * 100) / num_refs;
  int fault_pct = (faults * 100) / num_refs;

  printf("|   %d    | ", fr);
  printf("%s", algo);
  int len = strlen(algo);
  for (int s = 0; s < 9 - len; s++)
    printf(" ");
  printf(" |     %d     |   ", num_refs);
  print_pad(faults, 2);
  printf("   |  ");
  print_pad(hits, 2);
  printf("  |   ");
  print_pad(hit_pct, 2);
  printf("%%    |    ");
  print_pad(fault_pct, 2);
  printf("%%    |\n");
}

int main(int argc, char *argv[])
{
  printf("=== Question 4: Page Replacement Algorithms (FIFO, LRU, Optimal) ===\n\n");
  printf("Reference String (20 references):\n");
  for (int i = 0; i < num_refs; i++) {
    printf("%d ", ref_string[i]);
  }
  printf("\n\n");

  printf("============================================================\n");
  printf("Part 1: Detailed Step-by-Step Execution on 3 Frames\n");
  printf("============================================================\n\n");
  int f_fifo_3 = sim_fifo(3, 1);
  int f_lru_3 = sim_lru(3, 1);
  int f_opt_3 = sim_opt(3, 1);

  printf("============================================================\n");
  printf("Part 2: Comparison Across 3, 4, and 5 Frames\n");
  printf("============================================================\n\n");

  printf("+--------+-----------+------------+--------+------+-----------+------------+\n");
  printf("| Frames | Algorithm | References | Faults | Hits | Hit Ratio | Fault Rate |\n");
  printf("+--------+-----------+------------+--------+------+-----------+------------+\n");

  int frame_counts[3] = {3, 4, 5};
  for (int fc = 0; fc < 3; fc++) {
    int fr = frame_counts[fc];

    int faults_fifo = (fr == 3) ? f_fifo_3 : sim_fifo(fr, 0);
    print_table_row(fr, "FIFO", faults_fifo);

    int faults_lru = (fr == 3) ? f_lru_3 : sim_lru(fr, 0);
    print_table_row(fr, "LRU", faults_lru);

    int faults_opt = (fr == 3) ? f_opt_3 : sim_opt(fr, 0);
    print_table_row(fr, "Optimal", faults_opt);

    printf("+--------+-----------+------------+--------+------+-----------+------------+\n");
  }

  printf("\nPerformance Analysis:\n");
  printf("1. Optimal page replacement consistently yields the lowest page faults (9, 8, 7 faults)\n");
  printf("   because it possesses omniscient future lookahead (Belady optimal criterion).\n");
  printf("2. LRU closely tracks Optimal (12, 8, 7 faults) by leveraging temporal locality of access,\n");
  printf("   significantly outperforming FIFO on smaller frame capacities.\n");
  printf("3. FIFO performs worst (15, 10, 9 faults) because it evicts pages strictly based on arrival\n");
  printf("   order, often ejecting frequently referenced working-set pages.\n\n");

  printf("pagereplace completed successfully.\n");
  exit(0);
}
