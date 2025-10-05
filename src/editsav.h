#ifndef EDITSAV_H
#define EDITSAV_H

#include "pkmnstructs.h"

void calculate_checksum(FILE *fp);
bool write_checksum(FILE* fp, uint16_t checksum);

bool edit_money(FILE *fp, uint32_t amount);
bool complete_pokedex(FILE *fp);
bool max_item(FILE *fp, uint8_t item);
bool edit_iv_values(FILE *fp, pokemon *p, int iv, int val, int pokemon_location);
bool edit_xp_values(FILE *fp, pokemon *p, int stat_select, int pokemon_location, uint16_t xp);
bool edit_pokemon(FILE *fp, pokemon *p, int stat_selection, int pokemon_location, int stat_value);

#endif