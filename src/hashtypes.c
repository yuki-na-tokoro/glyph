/*
 * inspired by https://xnacly.me/posts/2024/c-hash-map/
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtypes.h"

const size_t BASE = 0x811c9dc5;
const size_t PRIME = 0x01000193;

// FNV-1a hash
size_t hash(char *key) {
  size_t hash = BASE;

  while(*key) {
    hash ^= *(key++);
    hash *= PRIME;
  }

  return hash;
}

struct map map_init(size_t cap) {
  struct map m;
  m.size = 0;
  m.cap = cap;
  m.buckets = calloc(cap, sizeof(struct entry));
  if (m.buckets == NULL) {
    perror("#init(): malloc error");
    exit(1);
  }
  return m;
}

void map_put(struct map *m, char *key, void *value) {
  size_t i = hash(key) & (m->cap - 1);

  if (m->buckets[i].value != NULL) {

    if (strcmp(key, m->buckets[i].key) == 0) {
      return;
    }

    m->cap *= 2;

    struct entry *old = m->buckets;

    m->buckets = calloc(m->cap, sizeof(struct entry));

    for (size_t j = 0; j < m->cap / 2; j++) {
      map_put(m, old[j].key, old[j].value);
    }

    map_put(m, key, value);

    free(old);
    
    return;
  }

  m->buckets[i].key = key;
  m->buckets[i].value = value;
  m->size++;
}

void* map_get(struct map *m, char *key) {
  size_t i = hash(key) & (m->cap - 1);

  if (m->buckets[i].value == NULL || strcmp(key, m->buckets[i].key) != 0) {
    return NULL;
  }

  return m->buckets[i].value;
}

void* map_remove(struct map *m, char *key) {
  size_t i = hash(key) & (m->cap - 1);

  if (m->buckets[i].value == NULL || strcmp(key, m->buckets[i].key) != 0) {
    return NULL;
  }

  void* value = m->buckets[i].value;
  m->buckets[i].key = NULL;
  m->buckets[i].value = NULL;
  m->size--;
  return value;
}

bool map_contains_key(struct map *m, char *key) {
  size_t i = hash(key) & (m->cap - 1);

  return !(m->buckets[i].value == NULL || strcmp(key, m->buckets[i].key) != 0);
}

struct set set_init(size_t cap) {
  struct set s;
  s.size = 0;
  s.cap = cap;
  s.buckets = calloc(cap, sizeof(char*));
  if (s.buckets == NULL) {
    perror("#init(): malloc error");
    exit(1);
  }

  return s;
}

void set_put(struct set *s, char *value) {
  size_t i = hash(value) & (s->cap - 1);

  if (s->buckets[i] != NULL) {

    s->cap *= 2;

    char **old = s->buckets;

    s->buckets = calloc(s->cap, sizeof(struct entry));

    for (size_t j = 0; j < s->cap / 2; j++) {
      set_put(s, old[j]);
    }

    set_put(s, value);

    free(old);
    
    return;
  }

  s->buckets[i] = value;
  s->size++;
}

void set_remove(struct set *s, char *value) {
  size_t i = hash(value) & (s->cap - 1);

  if (s->buckets[i] == NULL || strcmp(value, s->buckets[i]) != 0) {
    return;
  }

  s->buckets[i] = NULL;
  s->size--;
}

bool set_contains(struct set *s, char *value) {
  size_t i = hash(value) & (s->cap - 1);

  return !(s->buckets[i] == NULL || strcmp(value, s->buckets[i]) != 0);
}
