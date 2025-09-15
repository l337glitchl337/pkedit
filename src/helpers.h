#ifndef HELPERS_H
#define HELPERS_H

pokemon *load_party_pokemon(FILE *fp, uint8_t party_count);
item *load_bag_items(FILE *fp, int n);

#endif