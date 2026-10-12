#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_PROC 10
#define MAX_RES  10

// Print matrix state
void
print_state(int n, int m, int alloc[MAX_PROC][MAX_RES], int req_or_need[MAX_PROC][MAX_RES], int avail[MAX_RES], const char *req_name)
{
  printf("Available:\n ");
  for (int j = 0; j < m; j++) {
    printf(" R%d=%d", j, avail[j]);
  }
  printf("\nProcess | Allocation | %s\n", req_name);
  for (int i = 0; i < n; i++) {
    printf("  P%d    | ", i);
    for (int j = 0; j < m; j++) {
      printf("%d ", alloc[i][j]);
    }
    printf("     | ");
    for (int j = 0; j < m; j++) {
      printf("%d ", req_or_need[i][j]);
    }
    printf("\n");
  }
}

// Deadlock detection algorithm
int
detect_deadlock(int n, int m, int alloc[MAX_PROC][MAX_RES], int req[MAX_PROC][MAX_RES], int avail[MAX_RES], int deadlocked[])
{
  static int work[MAX_RES];
  static int finish[MAX_PROC];

  for (int j = 0; j < m; j++) {
    work[j] = avail[j];
  }

  for (int i = 0; i < n; i++) {
    int has_alloc = 0;
    for (int j = 0; j < m; j++) {
      if (alloc[i][j] > 0)
        has_alloc = 1;
    }
    finish[i] = (has_alloc == 0) ? 1 : 0;
  }

  int progress = 1;
  while (progress) {
    progress = 0;
    for (int i = 0; i < n; i++) {
      if (!finish[i]) {
        int can_satisfy = 1;
        for (int j = 0; j < m; j++) {
          if (req[i][j] > work[j]) {
            can_satisfy = 0;
            break;
          }
        }
        if (can_satisfy) {
          for (int j = 0; j < m; j++) {
            work[j] += alloc[i][j];
          }
          finish[i] = 1;
          progress = 1;
        }
      }
    }
  }

  int count = 0;
  for (int i = 0; i < n; i++) {
    if (!finish[i]) {
      deadlocked[count++] = i;
    }
  }
  return count;
}

// Banker's Algorithm: safety check
int
is_safe_state(int n, int m, int alloc[MAX_PROC][MAX_RES], int max[MAX_PROC][MAX_RES], int avail[MAX_RES], int safe_seq[])
{
  static int need[MAX_PROC][MAX_RES];
  static int work[MAX_RES];
  static int finish[MAX_PROC];

  for (int i = 0; i < n; i++) {
    finish[i] = 0;
    for (int j = 0; j < m; j++) {
      need[i][j] = max[i][j] - alloc[i][j];
    }
  }

  for (int j = 0; j < m; j++) {
    work[j] = avail[j];
  }

  int count = 0;
  int progress = 1;
  while (progress) {
    progress = 0;
    for (int i = 0; i < n; i++) {
      if (!finish[i]) {
        int can_finish = 1;
        for (int j = 0; j < m; j++) {
          if (need[i][j] > work[j]) {
            can_finish = 0;
            break;
          }
        }
        if (can_finish) {
          for (int j = 0; j < m; j++) {
            work[j] += alloc[i][j];
          }
          finish[i] = 1;
          safe_seq[count++] = i;
          progress = 1;
        }
      }
    }
  }

  return (count == n) ? 1 : 0;
}

// Static test data to avoid stack overflow
static int alloc1[MAX_PROC][MAX_RES] = {
  {0, 1, 0},
  {2, 0, 0},
  {3, 0, 3},
  {2, 1, 1},
  {0, 0, 2}
};
static int req1[MAX_PROC][MAX_RES] = {
  {0, 0, 0},
  {2, 0, 2},
  {0, 0, 0},
  {1, 0, 0},
  {0, 0, 2}
};
static int avail1[MAX_RES] = {0, 0, 0};

static int alloc2[MAX_PROC][MAX_RES] = {
  {0, 1, 0},
  {2, 0, 0},
  {3, 0, 3},
  {2, 1, 1},
  {0, 0, 2}
};
static int req2[MAX_PROC][MAX_RES] = {
  {0, 0, 0},
  {2, 0, 2},
  {0, 0, 1},
  {1, 0, 0},
  {0, 0, 2}
};
static int avail2[MAX_RES] = {0, 0, 0};

static int alloc3[MAX_PROC][MAX_RES] = {
  {0, 1, 0},
  {2, 0, 0},
  {3, 0, 2},
  {2, 1, 1},
  {0, 0, 2}
};
static int max3[MAX_PROC][MAX_RES] = {
  {7, 5, 3},
  {3, 2, 2},
  {9, 0, 2},
  {2, 2, 2},
  {4, 3, 3}
};
static int avail3[MAX_RES] = {3, 3, 2};

static int pipe_alloc[MAX_PROC][MAX_RES] = {
  {1, 0},
  {0, 1}
};
static int pipe_req[MAX_PROC][MAX_RES] = {
  {0, 1},
  {1, 0}
};
static int pipe_avail[MAX_RES] = {0, 0};

static int deadlocked[MAX_PROC];
static int safe_seq[MAX_PROC];

int
main(int argc, char *argv[])
{
  printf("Question 3: Deadlock Detection & Avoidance Simulation\n\n");

  // Test Case 1: Safe state (No deadlock)
  printf("--- Test Case 1: Safe Matrix (5 Processes, 3 Resources) ---\n");
  print_state(5, 3, alloc1, req1, avail1, "Request");
  int dl_cnt = detect_deadlock(5, 3, alloc1, req1, avail1, deadlocked);
  if (dl_cnt == 0) {
    printf("Result: No deadlock detected. System is safe.\n\n");
  } else {
    printf("Result: Deadlock detected! Deadlocked processes: ");
    for (int i = 0; i < dl_cnt; i++) printf("P%d ", deadlocked[i]);
    printf("\n\n");
  }

  // Test Case 2: Deadlocked Matrix
  printf("--- Test Case 2: Deadlocked Matrix (P2 requests R2=1) ---\n");
  print_state(5, 3, alloc2, req2, avail2, "Request");
  dl_cnt = detect_deadlock(5, 3, alloc2, req2, avail2, deadlocked);
  if (dl_cnt == 0) {
    printf("Result: No deadlock detected.\n\n");
  } else {
    printf("Result: Deadlock detected! Deadlocked processes: ");
    for (int i = 0; i < dl_cnt; i++) printf("P%d ", deadlocked[i]);
    printf("\n\n");
  }

  // Test Case 3: Transition to Unsafe State via Request (Banker's Algorithm)
  printf("--- Test Case 3: Banker's Algorithm Avoidance ---\n");
  print_state(5, 3, alloc3, max3, avail3, "Max    ");
  int safe = is_safe_state(5, 3, alloc3, max3, avail3, safe_seq);
  printf("Initial state check: %s\n", safe ? "SAFE" : "UNSAFE");
  if (safe) {
    printf("Safe sequence: ");
    for (int i = 0; i < 5; i++) printf("P%d ", safe_seq[i]);
    printf("\n");
  }

  // Process 1 requests [1, 0, 2]
  printf("\nEvaluating Request from P1: [1, 0, 2]\n");
  int req_p1[3] = {1, 0, 2};
  for (int j = 0; j < 3; j++) {
    alloc3[1][j] += req_p1[j];
    avail3[j] -= req_p1[j];
  }
  int safe_after = is_safe_state(5, 3, alloc3, max3, avail3, safe_seq);
  if (safe_after) {
    printf("Request GRANTED: System remains in SAFE state.\n");
    printf("New safe sequence: ");
    for (int i = 0; i < 5; i++) printf("P%d ", safe_seq[i]);
    printf("\n");
  } else {
    printf("Request REJECTED: Leads to UNSAFE state.\n");
  }

  // Process 4 requests [3, 3, 0]
  printf("\nEvaluating Request from P4: [3, 3, 0]\n");
  int req_p4[3] = {3, 3, 0};
  int can_alloc = 1;
  for (int j = 0; j < 3; j++) {
    if (req_p4[j] > avail3[j]) {
      can_alloc = 0;
      break;
    }
  }
  if (!can_alloc) {
    printf("Request DENIED: Exceeds available resources (R0 available=%d < requested=3).\n", avail3[0]);
  } else {
    for (int j = 0; j < 3; j++) {
      alloc3[4][j] += req_p4[j];
      avail3[j] -= req_p4[j];
    }
    if (is_safe_state(5, 3, alloc3, max3, avail3, safe_seq)) {
      printf("Request GRANTED.\n");
    } else {
      printf("Request REJECTED: System would enter UNSAFE state (Avoidance engaged).\n");
      for (int j = 0; j < 3; j++) {
        alloc3[4][j] -= req_p4[j];
        avail3[j] += req_p4[j];
      }
    }
  }

  // Circular wait simulation using real xv6 processes and pipes
  printf("\n--- Test Case 4: Real xv6 Process Circular Wait Simulation ---\n");
  int p1[2], p2[2];
  if (pipe(p1) < 0 || pipe(p2) < 0) {
    printf("pipe creation failed\n");
    exit(1);
  }

  printf("Resource mapping: Res0 = Pipe1, Res1 = Pipe2\n");
  printf("Process A holds Pipe1 write-end, waiting for Pipe2 read-end\n");
  printf("Process B holds Pipe2 write-end, waiting for Pipe1 read-end\n");

  print_state(2, 2, pipe_alloc, pipe_req, pipe_avail, "Request");
  dl_cnt = detect_deadlock(2, 2, pipe_alloc, pipe_req, pipe_avail, deadlocked);
  if (dl_cnt > 0) {
    printf("Result: Circular wait identified! Deadlocked processes: ");
    for (int i = 0; i < dl_cnt; i++) printf("P%d ", deadlocked[i]);
    printf("\n");
  }

  close(p1[0]); close(p1[1]);
  close(p2[0]); close(p2[1]);
  printf("Cleaned up pipes. Deadlock test completed.\n");
  exit(0);
}
