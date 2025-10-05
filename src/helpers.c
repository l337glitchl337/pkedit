#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pkmnstructs.h"
#include "helpers.h"
#include "offsets.h"
#include "species.h"

pokemon *load_party_pokemon(FILE *fp, uint8_t party_count)
{
    if(party_count > 6 || party_count < 1)
    {
        printf("Error while reading party size, expected 1-6 got %u\n", party_count);
        exit(1);
    }
    
    pokemon *party = malloc(sizeof(pokemon) * party_count);

    for(int i = 0; i < party_count; i++)
    {
        pokemon *p = load_pokemon(fp, true, 0, i+1, party_count);
        party[i] = *p;
        free(p);
    }

    return party;
}

item *load_bag_items(FILE *fp, int n)
{
    uint8_t byte;
    uint8_t item_count;
    item *bag_items = malloc(n * sizeof(item));

    if(!bag_items)
    {
        printf("Error allocating memory for bag items, quitting.\n");
        exit(1);
    }

    for(int i = 0; i < n; i++)
    {
        fread(&byte, 1, 1, fp);
        fread(&item_count, 1, 1, fp);
        bag_items[i].item_id = byte;
        bag_items[i].count = item_count;
    }

    return bag_items;
}

pokemon *load_box_pokemon(FILE *fp, int n)
{
    exit(1);
}

pokemon *load_pokemon(FILE *fp, bool party, int box, int slot, uint8_t party_count)
{
    pokemon *p = malloc(sizeof(pokemon));
    slot -= 1;
    long offset = 0;
    if(!p)
    {
        printf("Error allocating memory for PokeMon struct, quitting.\n");
        exit(1);
    }

    if(party)
    {
        fseek(fp, PARTY_OFFSET+1, SEEK_SET);
        offset = ((party_count + (slot * 44)) - slot) + (6 - party_count);
        if(party_count > 6 || party_count < 1 && party)
        {
            printf("Unexpected party count, quitting.\n");
            exit(1);
        }
    }
    else
    {
        uint8_t current_box = 0;
        uint8_t box_count = 0;
        fseek(fp, CURRENT_BOX_NUMBER, SEEK_SET);
        fread(&current_box, 1, 1, fp);
        current_box = current_box & 0x07;


        if(box-1 == current_box)
        {
            fseek(fp, CURRENT_BOX_OFFSET, SEEK_SET);
            fread(&box_count, 1, 1, fp);
        }
        else if(box <= 6)
        {
            printf ("box 1-6\n");
            fseek(fp, BOX_OFFSET_1_6, SEEK_SET);
            fread(&box_count, 1, 1, fp);
        }
        else if(box >= 7 && box <= 12)
        {
            fseek(fp, BOX_OFFSET_7_12, SEEK_SET);
            fread(&box_count, 1, 1, fp);
        }
        offset = (box_count + (slot * 33)) - slot;
    }

    fseek(fp, slot, SEEK_CUR);
    fread(&p->species, 1 ,1, fp);
    p->name = species[p->species];

    fseek(fp, offset+1, SEEK_CUR);

    p->offset_current_hp = ftell(fp);
    fread(&p->current_hp, 1, 2, fp);

    // skip trash level byte
    fseek(fp, 1, SEEK_CUR);

    //read status condition
    p->offset_status_cond = ftell(fp);
    fread(&p->status_cond, 1, 1, fp);

    //read pokemon type 1 and 2
    p->offset_type1 = ftell(fp);
    fread(&p->type1, 1, 1, fp);
    p->offset_type2 = ftell(fp);
    fread(&p->type2, 1, 1, fp);

    //read catch rate/held item
    p->offset_catch_rate_held_item = ftell(fp);
    fread(&p->catch_rate_held_item, 1, 1, fp);

    //read moves for pokemon
    p->offset_move1 = ftell(fp);
    fread(&p->move1, 1, 1, fp);
    p->offset_move2 = ftell(fp);
    fread(&p->move2, 1, 1, fp);
    p->offset_move3 = ftell(fp);
    fread(&p->move3, 1, 1, fp);
    p->offset_move4 = ftell(fp);
    fread(&p->move4, 1, 1, fp);

    //read OT ID (2 bytes)
    p->offset_orig_trainer_id = ftell(fp);
    fread(&p->orig_trainer_id, 1, 2, fp);

    //read exp points (3 bytes)
    p->offset_exp = ftell(fp);
    fread(&p->exp, 1, 3, fp);

    //read stat exp (hp, attack, defense, speed, special respectivelty)
    p->offset_hp_stat_exp = ftell(fp);
    fread(&p->hp_stat_exp, 1, 2, fp);
    p->offset_atk_stat_exp = ftell(fp);
    fread(&p->atk_stat_exp, 1, 2, fp);
    p->offset_def_stat_exp = ftell(fp);
    fread(&p->def_stat_exp, 1, 2, fp);
    p->offset_speed_stat_exp = ftell(fp);
    fread(&p->speed_stat_exp, 1, 2, fp);
    p->offset_special_stat_exp = ftell(fp);
    fread(&p->special_stat_exp, 1, 2, fp);

    //IV data
    p->offset_iv_data = ftell(fp);
    fread(&p->iv_data, 1, 2, fp);

    //PP values of moves 1-4 respectively
    p->offset_move1_pp = ftell(fp);
    fread(&p->move1_pp, 1, 1, fp);
    p->offset_move2_pp = ftell(fp);
    fread(&p->move2_pp, 1, 1, fp);
    p->offset_move3_pp = ftell(fp);
    fread(&p->move3_pp, 1, 1, fp);
    p->offset_move4_pp = ftell(fp);
    fread(&p->move4_pp, 1, 1, fp);

    if(party)
    {
        //level data
        p->offset_level = ftell(fp);
        fread(&p->level, 1, 1, fp);

        //Stats (max hp, attack, defense, speed, special respectively)
        p->offset_max_hp = ftell(fp);
        fread(&p->max_hp, 1, 2, fp);
        p->offset_atk = ftell(fp);
        fread(&p->atk, 1, 2, fp);
        p->offset_def = ftell(fp);
        fread(&p->def, 1, 2, fp);
        p->offset_speed = ftell(fp);
        fread(&p->speed, 1, 2, fp);
        p->offset_special = ftell(fp);
        fread(&p->special, 1, 2, fp);

         //stats from the pokemon
        p->cal_attack = (p->atk[0] << 8) | p->atk[1];
        p->cal_defense = (p->def[0] << 8) | p->def[1];
        p->cal_speed = (p->speed[0] << 8) | p->speed[1];
        p->cal_special = (p->special[0] << 8) | p->special[1];
    }

    p->cal_cur_hp = (p->current_hp[0] << 8) | p->current_hp[1];
    p->cal_ot_id = (p->orig_trainer_id[0] << 8) | p->orig_trainer_id[1];
    p->cal_exp = (p->exp[0] << 16) | (p->exp[1] << 8) | p->exp[2];
    p->cal_hp_xp = (p->hp_stat_exp[0] << 8) | p->hp_stat_exp[1];
    p->cal_atk_xp = (p->atk_stat_exp[0] << 8) | p->atk_stat_exp[1];
    p->cal_def_xp = (p->def_stat_exp[0] << 8) | p->def_stat_exp[1];
    p->cal_speed_xp = (p->speed_stat_exp[0] << 8) | p->speed_stat_exp[1];
    p->cal_special_xp = (p->special_stat_exp[0] << 8) | p->special_stat_exp[1];
    p->cal_max_hp = (p->max_hp[0] << 8) | p->max_hp[1];
    p->cal_iv_data = (p->iv_data[0] << 8) | p->iv_data[1];


    //calculating IV's byt doing some bit shifting and masking
    p->special_iv = p->cal_iv_data & 0xF;
    p->speed_iv = (p->cal_iv_data >> 4) & 0xF;
    p->defense_iv = (p->cal_iv_data >> 8) & 0xF;
    p->attack_iv = (p->cal_iv_data >> 12) & 0xF;

    //last IV is a bit of a pain in the arse to calculate
    //per the documentation, we need to take the LSB of each
    //IV calculated above, and then assemple it into a "binary string"
    //what im doing here is getting the LSB from each IV, then shifting them all over to pack them into 8 bits
    p->hp_iv = (p->attack_iv & 0x01) << 0 | (p->defense_iv & 0x01) << 1 | (p->speed_iv & 0x01) << 2 | (p->special_iv & 0x01) << 3;

    return p;
}

const PokemonBaseStats *get_base_stats(const char *name)
{
    for(int i = 0; i < 151; i++)
    {
        if(strcmp(pokedex[i].name, name) == 0)
        {
            return &pokedex[i];
        }
    }
    return NULL;
}

uint8_t get_level_from_exp(uint32_t exp, int group) {
    for (uint8_t level = 1; level <= 100; level++) {
        uint32_t required;
        switch (group) {
            case EXP_MEDIUM_FAST:
                required = level * level * level;
                break;
            case EXP_FAST:
                required = (4 * level * level * level) / 5;
                break;
            case EXP_SLOW:
                required = (5 * level * level * level) / 4;
                break;
            case EXP_MEDIUM_SLOW:
                required = (6 * level * level * level) / 5
                         - 15 * level * level
                         + 100 * level
                         - 140;
                if ((int32_t)required < 0) required = 0; // clamp
                break;
            default:
                return 0; // invalid group
        }

        if (required == exp) return level;
        if (required > exp)  return level - 1;
    }
    return 100;
}

bool calc_new_stat(FILE *fp, pokemon *p, int stat_selection)
{
    const PokemonBaseStats *base = get_base_stats(p->name);
    uint16_t new_stat = 0;

    switch (stat_selection)
    {
        case ATTACK:
            new_stat = (uint16_t)floor((((base->attack + p->attack_iv) * 2 + floor(sqrt((double)p->cal_atk_xp) / 4)) * p->level / 100) + 5);
            fseek(fp, p->offset_atk, SEEK_SET);
            break;
        case DEFENSE:
            new_stat = (uint16_t)floor((((base->defense + p->defense_iv) * 2 + floor(sqrt((double)p->cal_def_xp) / 4)) * p->level / 100) + 5);
            fseek(fp, p->offset_def, SEEK_SET);
            break;
        case SPEED:
            new_stat = (uint16_t)floor((((base->speed + p->speed_iv) * 2 + floor(sqrt((double)p->cal_speed_xp) / 4)) * p->level / 100) + 5);
            fseek(fp, p->offset_speed, SEEK_SET);
            break;
        case SPECIAL:
            new_stat = (uint16_t)floor((((base->special + p->special_iv) * 2 + floor(sqrt((double)p->cal_special_xp) / 4)) * p->level / 100) + 5);
            fseek(fp, p->offset_special, SEEK_SET);
            break;
        case HP:
            new_stat = (uint16_t)floor((((base->hp + p->hp_iv) * 2 + floor(sqrt((double)p->cal_hp_xp) / 4)) * p->level / 100) + p->level + 10);
            fseek(fp, p->offset_max_hp, SEEK_SET);
            break;
        default:
            return false;
    }

    uint8_t packed[2];
    packed[0] = (new_stat >> 8) & 0xFF;
    packed[1] = new_stat & 0xFF;
    if(fwrite(&packed, sizeof(packed), 1, fp) != 0)
    {
        return true;
    }
    return false;

}