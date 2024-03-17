#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "finwo/benchmark.h"
#include "mindex.h"

static void fn_purge(void *item, void *udata) {
  free(item);
  // Intentionally empty
}

static int fn_compare_string(const void *a, const void *b, void *udata) {
  const char *sa = (char*)a;
  const char *sb = (char*)b;
  return strcmp(sa, sb);
}

const char *alphabet = "0123456789abcdef";
int        alen      = sizeof(alphabet);
char *random_str(int length) {
  char       *str      = malloc(length + 1);
  for(int i=0; i<length; i++) {
    str[i] = alphabet[rand() % alen];
  }
  str[length] = '\0';
  return str;
}

void mindex_bmark_rstr_1024() {
  for(int i=0; i<1024; i++) {
    free(random_str(16));
  }
}

void mindex_bmark_rstr_65536() {
  for(int i=0; i<65536; i++) {
    free(random_str(16));
  }
}

void mindex_bmark_assign_1024() {
  struct mindex_t *mindex = mindex_init(fn_compare_string, fn_purge, NULL);
  for(int i=0; i<1024; i++) {
    mindex_set(mindex, random_str(16));
  }
  mindex_free(mindex);
}

void mindex_bmark_assign_65536() {
  struct mindex_t *mindex = mindex_init(fn_compare_string, fn_purge, NULL);
  for(int i=0; i<65536; i++) {
    mindex_set(mindex, random_str(16));
  }
  mindex_free(mindex);
}

int main() {

  char percentiles[] = {
    1, 5, 50, 95, 99, 0
  };

  BMARK(mindex_bmark_rstr_1024);
  BMARK(mindex_bmark_assign_1024);
  BMARK(mindex_bmark_rstr_65536);
  BMARK(mindex_bmark_assign_65536);
  return bmark_run(100, percentiles);
  return 0;
}
