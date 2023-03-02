#ifndef __FINWO_MINDEX_H__
#define __FINWO_MINDEX_H__

#include <stdlib.h>

struct mindex_t {
  int (*compare)(const void *a, const void *b, void *udata);
  void (*purge)(const void *item, void *udata);
  void *udata;
  size_t length;
  size_t spare;
  void **items;
};

struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void (*purge)(const void *item, void *udata), void *udata);
void              mindex_set(struct mindex_t *mindex, void *item);
void            * mindex_get(struct mindex_t *mindex, void *pattern);
void            * mindex_rand(struct mindex_t *mindex);
void              mindex_delete(struct mindex_t *mindex, void *item);
size_t            mindex_length(struct mindex_t *mindex);
void              mindex_free(struct mindex_t *mindex);

#endif // __FINWO_MINDEX_H__
