#ifndef PKMNSTRUCTS_H
#define PKMNSTRUCTS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct pokemon
{
    bool in_party;
    int slot_position;
    long offset;
    const char *name;
    uint8_t species;
    uint8_t current_hp[2];
    uint8_t status_cond;
    uint8_t type1;
    uint8_t type2;
    uint8_t catch_rate_held_item;
    uint8_t move1;
    uint8_t move2;
    uint8_t move3;
    uint8_t move4;
    uint8_t orig_trainer_id[2];
    uint8_t exp[3];
    uint8_t hp_stat_exp[2];
    uint8_t atk_stat_exp[2];
    uint8_t def_stat_exp[2];
    uint8_t speed_stat_exp[2];
    uint8_t special_stat_exp[2];
    uint8_t iv_data[2];
    uint8_t move1_pp;
    uint8_t move2_pp;
    uint8_t move3_pp;
    uint8_t move4_pp;
    uint8_t level;
    uint8_t max_hp[2];
    uint8_t atk[2];
    uint8_t def[2];
    uint8_t speed[2];
    uint8_t special[2];
} pokemon;

typedef struct item
{
    uint8_t item_id;
    uint8_t count;
} item;

#endif // PKMNSTRUCTS_H