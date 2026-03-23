/*
 * inspired by https://xnacly.me/posts/2024/c-hash-map/
 */

#ifndef HASHTYPES_H
#define HASHTYPES_H

#include <stddef.h>
#include <stdbool.h>

struct map {
  size_t size;
  size_t cap;
  struct entry *buckets;
};

struct entry {
  char *key;
  void *value;
};

const size_t BASE = 0x811c9dc5;
const size_t PRIME = 0x01000193;

size_t hash(char *key);

struct map map_init(size_t cap);

void map_put(struct map *m, char *key, void *value);

void* map_get(struct map *m, char *key);

void* map_remove(struct map *m, char *key);

bool map_contains_key(struct map *m, char *key);

struct set {
  size_t size;
  size_t cap;
  char **buckets;
};

struct set set_init(size_t cap);

void set_put(struct set *s, char *value);

void set_remove(struct set *s, char *value);

bool set_contains(struct set *s, char *value);

#endif // HASHTYPES_H
