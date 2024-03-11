#ifndef __FINWO_MINDEX_H__
#define __FINWO_MINDEX_H__

/// mindex
/// ======
///
/// In-memory indexed store and fetch library
///
/// This library makes use of [dep](https://github.com/finwo/dep) to manage it's
/// dependencies and exports.
///
/// Installation
/// ------------
///
/// ```sh
/// dep add finwo/mindex
/// dep install
/// ```
///
/// After that, simply add `include lib/.dep/config.mk` in your makefile and include
/// the header file by adding `#include "finwo/mindex.h`.
///
/// Features
/// --------
///
/// - Consistent interface, always working with pointers
/// - ANSI C (C99)
/// - Purge to be called when an item is removed

#include <stdlib.h>

/// API
/// ---


///
/// ### Structures
///

/// <details>
///   <summary>struct mindex_t</summary>
///
///   The main handle of the mindex instance
///<C
struct mindex_t {
  int (*compare)(const void *a, const void *b, void *udata);
  void (*purge)(void *item, void *udata);
  void *udata;
  size_t length;
  size_t max;
  void **items;
};
///>
/// </details>

///
/// ### Methods
///

/// <details>
///   <summary>mindex_init(cmp, purge, udata)</summary>
///
///   Initialize a new in-memory index
///<C
struct mindex_t * mindex_init(int (*compare)(const void *a, const void *b, void *udata), void (*purge)(void *item, void *udata), void *udata);
///>
/// </details>

/// <details>
///   <summary>mindex_find(mindex, pattern, items, length)</summary>
///
///   Intended for internal use or advanced usage (like fetching both index &
///   the pointer of the result)
///<C
struct mindex_find_response * mindex_find(const struct mindex_t *mindex, const void *pattern, void **items, int length);
///>
/// </details>

/// <details>
///   <summary>mindex_set(mindex, item)</summary>
///
///   Insert or replace an existing item
///<C
void mindex_set(struct mindex_t *mindex, void *item);
///>
/// </details>


/// <details>
///   <summary>mindex_get(mindex, pattern)</summary>
///
///   Simple query, fetch an entry in the index matching the pattern
///<C
void * mindex_get(struct mindex_t *mindex, void *pattern);
///>
/// </details>

/// <details>
///   <summary>mindex_nth(mindex, index)</summary>
///
///   Fetch the nth entry in the index
///<C
void * mindex_nth(struct mindex_t *mindex, int index);
///>
/// </details>

/// <details>
///   <summary>mindex_rand(mindex)</summary>
///
///   Retrieve a single random entry from the index
///<C
void * mindex_rand(struct mindex_t *mindex);
///>
/// </details>

/// <details>
///   <summary>mindex_delete(mindex, pattern)</summary>
///
///   Delete a single entry from the index matching the pattern
///<C
void mindex_delete(struct mindex_t *mindex, void *pattern);
///>
/// </details>

/// <details>
///   <summary>mindex_length(mindex)</summary>
///
///   The current amount of entries in the index
///<C
size_t mindex_length(struct mindex_t *mindex);
///>
/// </details>

/// <details>
///   <summary>mindex_free(mindex)</summary>
///
///   Purge all entries from the index and free the memory used
///<C
void mindex_free(struct mindex_t *mindex);
///>
/// </details>

#endif // __FINWO_MINDEX_H__

/// Testing
/// -------
///
/// If you want to run the library's tests, simply run `make test` to compile
/// the testing binary, and then `./test` to run the actual tests.
///
/// License
/// -------
///
/// mindex source code is available under the MIT license.
