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

extern const size_t BASE;
extern const size_t PRIME;

/*
 * @brief hashes a string
 */
size_t hash(char *key);

/*
 * @brief initializes a new map
 *
 * @param cap  initial capacity of the map (should be 2^n)
 */
struct map map_init(size_t cap);

/*
 * @brief puts a key-value pair into map m
 */
void map_put(struct map *m, char *key, void *value);

/*
 * @brief gets the associated value for key in map m
 */
void* map_get(struct map *m, char *key);

/*
 * @brief removes a key-value pair from map m
 */
void* map_remove(struct map *m, char *key);

/*
 * @brief checks if map m contains a key-value pair
 */
bool map_contains_key(struct map *m, char *key);

struct set {
  size_t size;
  size_t cap;
  char **buckets;
};

/*
 * @brief initializes a new set
 *
 * @param cap  initial capacity of the set (should be 2^n)
 */
struct set set_init(size_t cap);

/*
 * @brief puts a value into set s
 */
void set_put(struct set *s, char *value);

/*
 * @brief removes a value from set m
 */
void set_remove(struct set *s, char *value);

/*
 * @brief checks if set m contains a value
 */
bool set_contains(struct set *s, char *value);

#endif // HASHTYPES_H
