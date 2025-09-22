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

    //offsets
    uint32_t offset_species;
    uint32_t offset_current_hp;
    uint32_t offset_status_cond;
    uint32_t offset_type1;
    uint32_t offset_type2;
    uint32_t offset_catch_rate_held_item;
    uint32_t offset_move1;
    uint32_t offset_move2;
    uint32_t offset_move3;
    uint32_t offset_move4;
    uint32_t offset_orig_trainer_id;
    uint32_t offset_exp;
    uint32_t offset_hp_stat_exp;
    uint32_t offset_atk_stat_exp;
    uint32_t offset_def_stat_exp;
    uint32_t offset_speed_stat_exp;
    uint32_t offset_special_stat_exp;
    uint32_t offset_iv_data;
    uint32_t offset_move1_pp;
    uint32_t offset_move2_pp;
    uint32_t offset_move3_pp;
    uint32_t offset_move4_pp;
    uint32_t offset_level;
    uint32_t offset_max_hp;
    uint32_t offset_atk;
    uint32_t offset_def;
    uint32_t offset_speed;
    uint32_t offset_special;

    //DV/IVs
    uint8_t special_iv;
    uint8_t speed_iv;
    uint8_t defense_iv;
    uint8_t attack_iv;
    uint8_t hp_iv; 

    uint16_t cal_cur_hp;
    uint16_t cal_ot_id; 
    uint32_t cal_exp; 
    uint16_t cal_hp_xp; 
    uint16_t cal_atk_xp; 
    uint16_t cal_def_xp;
    uint16_t cal_speed_xp; 
    uint16_t cal_special_xp; 
    uint16_t cal_max_hp;

    //stats from the pokemon
    uint16_t cal_attack;
    uint16_t cal_defense;
    uint16_t cal_speed;
    uint16_t cal_special;

    uint16_t cal_iv_data;
} pokemon;

typedef struct item
{
    uint8_t item_id;
    uint8_t count;
} item;


typedef struct PokemonBaseStats{
    const char *name;
    int hp;
    int attack;
    int defense;
    int speed;
    int special;
    int total;
    float avg;
} PokemonBaseStats;

extern const PokemonBaseStats pokedex[];

#endif // PKMNSTRUCTS_H