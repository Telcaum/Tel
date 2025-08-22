#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#define TEL_ALLOCN(T, n) (T *)calloc((n), sizeof(T))
#define TEL_ALLOC(T) TEL_ALLOCN(T, 1)
#define TEL_REALLOCN(ptr, T, n) (T *)realloc((ptr), (n) * sizeof(T))
#define TEL_FREE(ptr) free(ptr)


#define TEL_CHECK_OOM(ptr) ( \
  ptr == NULL ? \
    (fprintf(stderr, "%s:%d: Out of memory\n", __FILE__, __LINE__), exit(1)) : \
    (void)0 \
)


#define TEL_LIST_INITIAL_CAPACITY 8

#define TEL_LIST(T) struct { \
  int length; \
  int capacity; \
  T *data; \
}

typedef TEL_LIST(int) Tel_IntList;

#define TEL_LIST_INIT(l, T) ( \
  assert(sizeof(T) == sizeof(*(l)->data)), \
  (l)->data = (T *)calloc(TEL_LIST_INITIAL_CAPACITY, sizeof(*(l)->data)), \
  TEL_CHECK_OOM((l)->data), \
  (l)->length = 0, (l)->capacity = TEL_LIST_INITIAL_CAPACITY \
)

#define _TEL_LIST_GROW(l, T) ( \
  (l)->data = (T *) \
    realloc((l)->data, ((l)->capacity *= 2) * sizeof(*(l)->data)), \
  TEL_CHECK_OOM((l)->data) \
)

#define TEL_LIST_PUSH(l, v, T) ( \
  (l)->length == (l)->capacity ? _TEL_LIST_GROW(l, T) : (void)0, \
  (l)->data[(l)->length++] = (v)  \
)

#define TEL_LIST_POP(l) ((l)->data[--(l)->length])

#define TEL_LIST_FREE(l) (free((l)->data), (l)->data = NULL, (l)->length = 0, (l)->capacity = 0)
