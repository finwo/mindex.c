#include <stdlib.h>
#include <string.h>

#include "finwo/assert.h"
#include "mindex.h"

static void fn_purge(const void *item, void *udata) {
  // Intentionally empty
}

static int fn_compare(const void *a, const void *b, void *udata) {
  const char *sa = a;
  const char *sb = b;
  return strcmp(sa, sb);
}

char *test_a00 = "Hello World";
char *test_a01 = "Hello World";
char *test_a02 = "Foobar";

void test_mindex_basics() {
  struct mindex_t *mindex = mindex_init(fn_compare, fn_purge, NULL);

  ASSERT("mindex_init returns non-null", mindex != NULL);

  mindex_set(mindex, test_a01);
  mindex_set(mindex, test_a00);
  mindex_set(mindex, test_a02);

  ASSERT("length = 2 after inserting 3 strings with 1 duplicate", mindex_length(mindex) == 2);

  char *found = mindex_get(mindex, test_a00);

  ASSERT("Fetching by original = get original", found == test_a00);

  found = mindex_get(mindex, test_a01);

  ASSERT("Fetching by same content = get original", found == test_a00);

  found = mindex_get(mindex, test_a02);

  ASSERT("Fetching by other = get other", found == test_a02);

  mindex_set(mindex, test_a01);
  found = mindex_get(mindex, test_a00);

  ASSERT("Overridden get by org returns new", found == test_a01);

  found = mindex_get(mindex, test_a01);

  ASSERT("Overridden get by new returns new", found == test_a01);
}

int main() {

  // Seed random
  unsigned int seed;
  FILE* urandom = fopen("/dev/urandom", "r");
  fread(&seed, sizeof(int), 1, urandom);
  fclose(urandom);
  srand(seed);

  // Run the actual tests
  RUN(test_mindex_basics);
  return TEST_REPORT();
}
