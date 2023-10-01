#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "finwo/assert.h"
#include "mindex.h"

static void fn_purge(void *item, void *udata) {
  // Intentionally empty
}

static int fn_compare_string(const void *a, const void *b, void *udata) {
  const char *sa = (char*)a;
  const char *sb = (char*)b;
  return strcmp(sa, sb);
}

struct character {
  char *name;
  char *description;
  char *species;
};

static int fn_compare_struct(const void *a, const void *b, void *udata) {
  const struct character *ca = a;
  const struct character *cb = b;
  return strcmp(ca->name, cb->name);
}

void test_mindex_string() {
  char *test_a00 = "Hello World";
  char *test_a01 = calloc(1, strlen(test_a00)+1);
  char *test_a02 = "Foobar";
  strcpy(test_a01, test_a00);

  ASSERT("test_a00 != test_a01", test_a00 != test_a01);
  ASSERT("test_a01 != test_a02", test_a01 != test_a02);
  ASSERT("test_a00 != test_a02", test_a00 != test_a02);

  struct mindex_t *mindex = mindex_init(fn_compare_string, fn_purge, NULL);
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

  mindex_free(mindex);
}

void test_mindex_structs() {
  struct mindex_t *mindex = mindex_init(fn_compare_struct, fn_purge, NULL);

  // Insert data
  mindex_set(mindex, &(struct character){ .name= "C-3PO"           , .description= "Protocol droid."                , .species= "Droid" });
  mindex_set(mindex, &(struct character){ .name= "R2-D2"           , .description= "Astromech droid built on Naboo.", .species= "Droid" });
  mindex_set(mindex, &(struct character){ .name= "Anakin Skywalker", .description= "Fallen Jedi, the chosen one."   , .species= "Human" });
  mindex_set(mindex, &(struct character){ .name= "Obi-Wan Kenobi"  , .description= "Jedi Master."                   , .species= "Human" });
  mindex_set(mindex, &(struct character){ .name= "Moon Moon"       , .description= "Mentally challenged wolf."      , .species= "Wolf"  });
  ASSERT("Length = 5", mindex_length(mindex) == 5);

  // Check fetching a bunch of data
  struct character *found = mindex_get(mindex, &(struct character){ .name="Moon Moon" });
  if (!found) return;

  // Check if the jedi master can be fetched
  ASSERT("{Obi-Wan Kenobi}.description = Jedi Master", strcmp(((struct character *)mindex_get(mindex, &(struct character){ .name="Obi-Wan Kenobi" }))->description, "Jedi Master.") == 0);
  ASSERT("{C-3PO}.species = Droid", strcmp(((struct character *)mindex_get(mindex, &(struct character){ .name="C-3PO" }))->species, "Droid") == 0);

}

int main() {

  // Seed random
  unsigned int seed;
  FILE* urandom = fopen("/dev/urandom", "r");
  fread(&seed, sizeof(int), 1, urandom);
  fclose(urandom);
  srand(seed);

  // Run the actual tests
  RUN(test_mindex_string);
  RUN(test_mindex_structs);
  return TEST_REPORT();
}
