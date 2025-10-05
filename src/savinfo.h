#ifndef SAVINFO_H
#define SAVINFO_H

#include "pkmnstructs.h"

void show_sav_summary(FILE *fp, int mode);
void get_player_name(FILE *fp);
void get_player_money(FILE *fp);
void get_player_id(FILE *fp);
void get_hof_entries(FILE *fp);
void get_play_time(FILE *fp);
void get_bag_items(FILE *fp);
void get_party_members(FILE *fp);
void get_player_badges(FILE *fp);
void get_pokedex_summary(FILE *fp);
void show_box_pokemon(FILE *fp);
void show_pokemon_summary(FILE *fp, pokemon *p, PokemonLocation location);

#endif