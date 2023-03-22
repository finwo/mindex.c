#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* usleep(): Sleep for the requested number of microseconds. */
int musleep(long usec) {
    struct timespec ts;
    int res;

    if (usec < 0) {
        return -1;
    }

    ts.tv_sec = usec / 1000000;
    ts.tv_nsec = (usec % 1000000) * 1000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

#define BMARK(fn) bmark_enqueue((#fn), (fn))
#define MIN(a,b) ((a<b)?(a):(b))
#define MAX(a,b) ((a>b)?(a):(b))

struct bmark_queue_t {
  void *next;
  char *name;
  void (*fn)();
};
struct bmark_ptile_t {
  char n;
};

struct bmark_queue_t *bmark_queue   = NULL;
char                  bmark_tiles[] = {
  1,
  10,
  50,
  90,
  99,
  0,
};

char *units[] = {
  "us",
  "ms",
  "s",
  NULL,
};

const char *header_description = "Description";

void bmark_enqueue(char *name, void (*fn)()) {
  struct bmark_queue_t *q = calloc(1, sizeof(struct bmark_queue_t));
  q->name = name;
  q->fn   = fn;
  q->next = bmark_queue;
  bmark_queue = q;
}

// Example time numbers:  12.23 s
//                       900.00 ms
//                       123.45 us
int bmark_run(int run_count) {
  int header_desclen = strlen(header_description);
  int i, r, unit;
  struct timeval tv_start;
  struct timeval tv_end;

  uint64_t *runs    = calloc(run_count, sizeof(uint64_t));
  uint64_t  run_tmp = 0;
  double    run_f   = 0;

  // Get max description length
  struct bmark_queue_t *q_entry = bmark_queue;
  while(q_entry) {
    header_desclen = MAX(header_desclen, strlen(q_entry->name));
    q_entry = q_entry->next;
  }

  printf("\n");

  // Separator
  printf("+-");
  for(i = 0 ; i < header_desclen ; i++) printf("-");
  for(i = 0 ; bmark_tiles[i] ; i++) printf("-+----------");
  printf("-+\n");

  // Column names
  printf("| %-*s |", header_desclen, header_description);
  for(i = 0 ; bmark_tiles[i] ; i++) printf(" %*d %% |", 7, bmark_tiles[i]);
  printf("\n");

  // Separator
  printf("+-");
  for(i = 0 ; i < header_desclen ; i++) printf("-");
  for(i = 0 ; bmark_tiles[i] ; i++) printf("-+----------");
  printf("-+\n");

  // Go over the whole queue
  q_entry = bmark_queue;
  while(q_entry) {
    printf("| %-*s |", header_desclen, q_entry->name);
    fflush(stdout);

    // Do the actual runs
    for(r = 0; r < run_count ; r++) {
      gettimeofday(&tv_start, NULL);
      q_entry->fn();
      gettimeofday(&tv_end, NULL);
      runs[r] = (tv_end.tv_sec*(uint64_t)1000000+tv_end.tv_usec) - (tv_start.tv_sec*(uint64_t)1000000+tv_start.tv_usec);
    }

    // Sort the runs
    // Basic bubble sort is fine here, we're not going for a speed record
    for(r = 0; r < (run_count-1) ; r++) {
      if (r < 0) continue;
      if (runs[r] > runs[r+1]) {
        run_tmp = runs[r];
        runs[r] = runs[r+1];
        runs[r+1] = run_tmp;
        r -= 2;
      }
    }

    // Print percentiles
    for(i = 0 ; bmark_tiles[i] ; i++) {
      r     = bmark_tiles[i] * run_count / 100;
      run_f = (double)runs[r];
      unit  = 0;
      while((run_f > 1000) && units[unit+1]) {
        run_f = run_f / 1000;
        unit++;
      }
      printf(" %*.2lf %2s |", 6, run_f, units[unit]);
    }

    printf("\n");

    q_entry = q_entry->next;
  }

  // Separator
  printf("+-");
  for(i = 0 ; i < header_desclen ; i++) printf("-");
  for(i = 0 ; bmark_tiles[i] ; i++) printf("-+----------");
  printf("-+\n");

  printf("\n");
  return 0;
}


void mindex_some_bmark_method() {
  // Intentionally empty
  musleep(100);
}

int main() {
  BMARK(mindex_some_bmark_method);
  return bmark_run(10000);
}
