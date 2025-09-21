#ifndef HELPERS_H
#define HELPERS_H

#include "pkmnstructs.h"

pokemon *load_party_pokemon(FILE *fp, uint8_t party_count);
item *load_bag_items(FILE *fp, int n);
pokemon *load_pokemon(FILE *fp, bool party, int slot);
const PokemonBaseStats *get_base_stats(const char *name);

#endif