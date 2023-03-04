mindex
======

In-memory indexed store and fetch library

This library makes use of [dep](https://github.com/finwo/dep) to manage it's
dependencies and exports.

Installation
------------

```sh
dep add finwo/mindex
dep install
```

After that, simply add `include lib/.dep/config.mk` in your makefile and include
the header file by adding `#include "finwo/mindex.h`.

Features
--------

- Consistent interface, always working with pointers
- ANSI C (C99)
- Purge to be called when an item is removed

Example
-------

```c
int main() {
  // TODO
}
```

API
---

### Basic

```c
struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void (*purge)(const void *item, void *udata), void *udata);
```

Initialize the indexing library.

```c
void              mindex_set(struct mindex_t *mindex, void *item);
```

Insert or replace an existing item

```c
void            * mindex_get(struct mindex_t *mindex, void *pattern);
```

Get an existing item

```c
void              mindex_delete(struct mindex_t *mindex, void *pattern);
```

Delete an item from the index

**:warning: CAUTION :warning:**: Deletes by finding in the index with the
compare fn, not by the pointer of the given item/pattern.

```c
size_t            mindex_length(struct mindex_t *mindex);
```

Returns the amount of items in the index

```c
void              mindex_free(struct mindex_t *mindex);
```

Release the memory used by the index

### Extended

```c
void            * mindex_rand(struct mindex_t *mindex);
```

Retreive a random item from the index

Testing
-------

If you want to run the library's tests, simply run `make test` to compile the
testing binary, and then `./test` to run the actual tests.

License
-------

mindex source code is available under the MIT license.
