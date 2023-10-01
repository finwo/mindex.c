#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include "mindex.h"

#ifndef MINDEX_SPARE
#define MINDEX_SPARE 16
#endif

int mindex_signal_found = 1;
int mindex_signal_over  = 2;
int mindex_signal_under = 4;
int mindex_signal_empty = 8;

struct mindex_find_response {
  int signal;
  int index;
  void *value; // NULL = not found
};

struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void (*purge)(void *item, void *udata), void *udata) {
  struct mindex_t *mindex = malloc(sizeof(struct mindex_t));
  mindex->compare = compare;
  mindex->purge   = purge;
  mindex->udata   = udata;
  mindex->items   = malloc(MINDEX_SPARE * sizeof(void*));
  mindex->length  = 0;
  mindex->max     = MINDEX_SPARE;
  return mindex;
}

struct mindex_find_response * mindex_find(const struct mindex_t *mindex, const void *pattern, void **items, int length) {
  int acc, idx, newlength;
  struct mindex_find_response *resp;

  // Nothing to search = done
  if (length <= 0) {
    resp         = malloc(sizeof(struct mindex_find_response));
    resp->signal = mindex_signal_empty;
    resp->index  = 0;
    resp->value  = NULL;
    return resp;
  }

  // Make the actual comparison, splitting the array in 2
  idx = length / 2;
  acc = mindex->compare(pattern, items[idx], mindex->udata);

  // Pattern = below middlepoint
  if (acc < 0) {

    // Limit deeper search to only the left side
    newlength = idx;

    // Nothing to our left, item not found
    if (newlength <= 0) {
      resp         = malloc(sizeof(struct mindex_find_response));
      resp->signal = mindex_signal_under;
      resp->index  = 0;
      resp->value  = NULL;
      return resp;
    }

    // Limit deeper search to only the left side
    newlength = idx;

    // Run the find on items below
    resp = mindex_find(mindex, pattern, items, newlength);

    // Surpress "over" signal
    resp->signal &= ~mindex_signal_over;

    return resp;
  }

  // Pattern = above middlepoint
  if (acc > 0) {

    // Calculate new index to check
    newlength = length - 1 - idx;

    // Nothing to our right, item not found
    if (newlength <= 0) {
      resp         = malloc(sizeof(struct mindex_find_response));
      resp->signal = mindex_signal_over;
      resp->index  = length;
      resp->value  = NULL;
      return resp;
    }

    // Get stats on the right-side of our pivot & translate response index
    resp = mindex_find(mindex, pattern, &(items[idx+1]), newlength);
    resp->index += idx + 1;

    // Surpress "under" signal
    resp->signal &= ~mindex_signal_under;

    return resp;
  }

  // Found = win
  resp         = malloc(sizeof(struct mindex_find_response));
  resp->signal = mindex_signal_found;
  resp->index  = idx;
  resp->value  = items[idx];
  return resp;
}

void mindex_set(struct mindex_t *mindex, void *item) {
  size_t movesize;

  // Ensure we have space to insert a new item
  if (mindex->max == mindex->length) {
    mindex->max   = mindex->max * 2;
    mindex->items = realloc(mindex->items, mindex->max * sizeof(void*));
  }

  // Find the index to insert (or already-occupied pattern)
  struct mindex_find_response *resp = mindex_find(mindex, item, mindex->items, mindex->length);

  // Item already in there, notify we skipped insertion
  if (resp->signal & mindex_signal_found) {
    mindex->items[resp->index] = item;
    mindex->purge(item, mindex->udata);
    free(resp);
    return;
  }

  // Move items above aside for the insertion
  // Essentially single-step insertion sort
  movesize = (mindex->length - resp->index) * sizeof(void*);
  memmove(&(mindex->items[resp->index + 1]), &(mindex->items[resp->index]), movesize);
  mindex->items[resp->index] = item;
  mindex->length++;

  // Aanndd.. We're done
  free(resp);
}

void * mindex_get(struct mindex_t *mindex, void *pattern) {
  struct mindex_find_response *resp = mindex_find(mindex, pattern, mindex->items, mindex->length);
  void *value = resp->value;
  free(resp);
  return value;
}

void * mindex_rand(struct mindex_t *mindex) {
  if (!mindex->length) {
    return NULL;
  }
  return mindex->items[rand() % mindex->length];
}

void mindex_delete(struct mindex_t *mindex, void *pattern) {
  struct mindex_find_response *resp = mindex_find(mindex, pattern, mindex->items, mindex->length);

  // Not found = done
  if (!(resp->signal & mindex_signal_found)) {
    free(resp);
    return;
  }

  // Call user's purge method
  void *item = mindex->items[resp->index];
  mindex->purge(item, mindex->udata);

  // Move everything on it's right to it
  void *dst = &(mindex->items[resp->index]);
  void *src = &(mindex->items[resp->index+1]);
  memmove(dst, src, (mindex->length - resp->index - 1) * sizeof(void*));

  // Update our size tracker
  mindex->length--;

  // TODO: reduce memory usage?

  // Done
  free(resp);
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
