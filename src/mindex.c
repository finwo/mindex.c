#include "mindex.h"

struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void *udata) {
  struct mindex_t *mindex = malloc(sizeof(struct mindex_t));
  mindex->compare = compare;
  mindex->items   = NULL;
  mindex->length  = 0;
  return NULL;
}

void mindex_set(struct mindex_t *mindex, void *item) {

}

void * mindex_get(struct mindex_t *mindex, void *pattern) {
  return NULL;
}

void * mindex_rand(struct mindex_t *mindex) {
  return NULL;
}

void mindex_delete(struct mindex_t *mindex, void *item) {

}

size_t mindex_length(struct mindex_t *mindex) {
  return 0;
}

void mindex_free(struct mindex_t *mindex) {

}
