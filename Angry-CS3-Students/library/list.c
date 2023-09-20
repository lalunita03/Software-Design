#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const size_t FACTOR = 2;
const size_t DEFAULT = 20;

typedef struct list {
  size_t size;
  size_t max_size;
  void **gen_array;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *retlist = malloc(sizeof(list_t));
  assert(retlist != NULL);
  retlist->gen_array = malloc(sizeof(void *) * initial_size);
  assert(retlist->gen_array != NULL);

  retlist->size = 0;
  retlist->max_size = initial_size;
  retlist->freer = freer;
  return retlist;
}

void list_free2(list_t *list) {
  free(list->gen_array);
  free(list);
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list->size; i++) {
      (list->freer)(list->gen_array[i]);
    }
  }
  free(list->gen_array);
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size);
  return list->gen_array[index];
}

void *list_remove(list_t *list, size_t index) {
  assert(index < list->size);
  assert(index >= 0);
  assert(list->size != 0);
  void *removed = list->gen_array[index];
  for (size_t i = index; i < list->size - 1; i++) {
    list->gen_array[i] = list->gen_array[i + 1];
  }

  list->size--;
  return removed;
}

void list_grow(list_t *list) {
  list_t *old = list;
  void **gen_arr = malloc(sizeof(void *) * list->max_size * FACTOR);
  for (int i = 0; i < list->size; i++) {
    gen_arr[i] = list_get(list, i);
  }

  free(old->gen_array);
  list->gen_array = gen_arr;
  list->max_size = list->max_size * FACTOR;
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);

  if (list->size == list->max_size) {
    list_grow(list);
  }
  list->gen_array[list->size] = value;
  list->size++;
}
