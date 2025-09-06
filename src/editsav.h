#ifndef EDITSAV_H
#define EDITSAV_H

uint16_t calculate_checksum(FILE *fp);
bool edit_money(FILE *fp, uint32_t amount);

#endif