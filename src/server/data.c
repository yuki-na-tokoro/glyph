
#include "data.h"

struct map chn_map;

void init_channel_map(void) {
  chn_map = map_init(64);
}

struct map* get_channel_map(void) {
  return &chn_map;
}

