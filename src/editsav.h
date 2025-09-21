#ifndef EDITSAV_H
#define EDITSAV_H

void calculate_checksum(FILE *fp);
bool write_checksum(FILE* fp, uint16_t checksum);
bool edit_money(FILE *fp, uint32_t amount);
bool complete_pokedex(FILE *fp);
bool max_item(FILE *fp, uint8_t item);
bool edit_attack_xp(FILE *fp, bool party, int slot, int xp);
bool edit_def_xp(FILE *fp, bool party, int slot, int xp);
bool edit_speed_xp(FILE *fp, bool party, int slot, int xp);
bool edit_special_xp(FILE *fp, bool party, int slot, int xp);
bool edit_hp_xp(FILE *fp, bool party, int slot, int xp);
bool edit_xp_stat(FILE *fp, int stat_select, bool party, int slot, int xp);


#endif