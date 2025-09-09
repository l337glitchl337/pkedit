#ifndef EDITSAV_H
#define EDITSAV_H

uint16_t calculate_checksum(FILE *fp);
bool write_checksum(FILE* fp, uint16_t checksum);
bool edit_money(FILE *fp, uint32_t amount);
bool complete_pokedex(FILE *fp);
bool max_item(FILE *fp, uint8_t item);

#endif