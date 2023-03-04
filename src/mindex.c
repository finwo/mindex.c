#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include "noporpoise/sort_r.h"

#include "mindex.h"

#ifndef MINDEX_SPARE
#define MINDEX_SPARE 16
#endif

struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void (*purge)(const void *item, void *udata), void *udata) {
  struct mindex_t *mindex = malloc(sizeof(struct mindex_t));
  mindex->compare = compare;
  mindex->purge   = purge;
  mindex->udata   = udata;
  mindex->items   = malloc(MINDEX_SPARE * sizeof(void*));
  mindex->length  = 0;
  mindex->spare   = MINDEX_SPARE;
  return mindex;
}

void mindex_set(struct mindex_t *mindex, void *item) {

  // Check if the item is already in the list
  void *found = mindex_get(mindex, item);

  // The exact pointer already there = done
  if (found && (item == found)) {
    return;
  }

  // Different pointer, so remove from the list
  // Indicate to lib user that it needs to be purged from mem
  if (found) {
    mindex_delete(mindex, item);
  }

  // Allocate more space if we ran out
  if (!mindex->spare) {
    mindex->items = realloc(mindex->items, (mindex->length + MINDEX_SPARE) * sizeof(void*));
    mindex->spare = MINDEX_SPARE;
  }

  // Add the new item to the end of the list
  mindex->items[mindex->length] = item;
  mindex->length++;
  mindex->spare--;

  // Sort the list
  sort_r(mindex->items, mindex->length, sizeof(void *), mindex->compare, mindex->udata);

  // Done
}

// Returns the INDEX within the items, not the item itself
int mindex_get_internal(void **items, size_t length, void *pattern, int (*compare)(const void *a, const void *b, void *udata), void *udata) {
  int sub;

  // No entries = not found
  if (length < 1) {
    return -1;
  }

  // Test the middle of the array
  size_t testIdx = length / 2;
  int compare_result = compare(items[testIdx], pattern, udata);

  // Match = found
  if (compare_result == 0) {
    return testIdx;
  }

  // Pattern = to the right
  if (compare_result < 0) {
    sub = mindex_get_internal(
      &(items[testIdx+1]),
      length - testIdx - 1,
      pattern,
      compare,
      udata
    );

    // Pass along error code
    if (sub < 0) {
      return sub;
    }

    // Return mapped location
    return sub + testIdx + 1;
  }

  // Pattern to the left
  if (compare_result > 0) {
    return mindex_get_internal(
      items,
      testIdx,
      pattern,
      compare,
      udata
    );
  }

  // Should never be called
  return -2;
}

void * mindex_get(struct mindex_t *mindex, void *pattern) {
  int idx = mindex_get_internal(mindex->items, mindex->length, pattern, mindex->compare, mindex->udata);

  // Not found
  if (idx < 0) {
    return NULL;
  }

  return mindex->items[idx];
}

void * mindex_rand(struct mindex_t *mindex) {
  if (!mindex->length) {
    return NULL;
  }
  return mindex->items[rand() % mindex->length];
}

// TODO: mindex_delete_internal(..., items, idx)
// TODO: delete an exact pointer, not a pattern (extra fn?)

void mindex_delete(struct mindex_t *mindex, void *pattern) {
  int idx = mindex_get_internal(mindex->items, mindex->length, pattern, mindex->compare, mindex->udata);
  void *item = mindex->items[idx];

  // Purge if not an exact match
  mindex->purge(item, mindex->udata);

  // Move everything on it's right to it
  void *dst = &(mindex->items[idx]);
  void *src = &(mindex->items[idx+1]);
  memmove(dst, src, mindex->length - idx - 1);

  // Update our size trackers
  mindex->length--;
  mindex->spare++;

  // Free some memory if we have a lot of spares
  if (mindex->spare > (MINDEX_SPARE*2)) {
    mindex->items = realloc(mindex->items, (mindex->length + MINDEX_SPARE) * sizeof(void*));
    mindex->spare = MINDEX_SPARE;
  }

  // Done
}

size_t mindex_length(struct mindex_t *mindex) {
  return mindex->length;
}

void mindex_free(struct mindex_t *mindex) {

  // Step 1, purge all entries
  int i;
  for (i=0; i < mindex->length; i++) {
    mindex->purge(mindex->items[i], mindex->udata);
  }

  // Step 2, free the list
  free(mindex->items);

  // Step 3, free the mindex itself
  free(mindex);

  // Done
}
