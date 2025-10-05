#ifndef HELPERS_H
#define HELPERS_H

#include "pkmnstructs.h"

pokemon *load_party_pokemon(FILE *fp, uint8_t party_count);
item *load_bag_items(FILE *fp, int n);
pokemon *load_pokemon(FILE *fp, bool party, int box, int slot, uint8_t party_count);
const PokemonBaseStats *get_base_stats(const char *name);
uint8_t get_level_from_exp(uint32_t exp, int group);
bool calc_new_stat(FILE *fp, pokemon *p, int stat_selection);

#endif