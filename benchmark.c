#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "finwo/benchmark.h"

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

void mindex_some_bmark_method() {
  // Intentionally empty
  musleep(100);
}

int main() {

  char percentiles[] = {
    1, 5, 50, 95, 99, 0
  };

  BMARK(mindex_some_bmark_method);
  return bmark_run(10000, percentiles);
}
