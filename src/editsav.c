#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "editsav.h"
#include "offsets.h"
#include "items.h"
#include "helpers.h"
#include "pkmnstructs.h"

#define BUFF_SIZE 3979

void calculate_checksum(FILE *fp)
{
    
    uint16_t calculated_checksum = 0;
    // 1 byte buffer with BUFF_ZIZE (3979 bytes)
    uint8_t buffer[BUFF_SIZE];

    int len = sizeof(buffer) / sizeof(buffer[0]);

    fseek(fp, SAVE_START_OFFFSET, SEEK_SET);

    fread(&buffer, sizeof(buffer), 1, fp);

    for(int i = 0; i <= len; i++)
    {
        //only worry about the bottom 8 bits
        calculated_checksum += buffer[i] & 0xFF;
    }

    //invert and get last 8 bits
    calculated_checksum = (~calculated_checksum) & 0xFF;
    if(!write_checksum(fp, calculated_checksum))
    {
        printf("Error: Could not write checksum, quitting.");
        exit(1);
    }
}

bool write_checksum(FILE* fp, uint16_t checksum)
{
    int seek = fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
    int write = fwrite(&checksum, sizeof(checksum), 1, fp);

    if(seek == 0 && write != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool edit_money(FILE *fp, uint32_t amount)
{
    uint8_t bcd_bytes[3];
    fseek(fp, MONEY_OFFSET, SEEK_SET);

    int d0 = amount % 10;
    amount /= 10;
    int d1 = amount % 10;
    amount /= 10;
    int d2 = amount % 10;
    amount /= 10;
    int d3 = amount % 10;
    amount /= 10;
    int d4 = amount % 10;
    amount /= 10;
    int d5 = amount % 10;

    bcd_bytes[0] = (d5 << 4) | d4; 
    bcd_bytes[1] = (d3 << 4) | d2;
    bcd_bytes[2] = (d1 << 4) | d0;

    fwrite(&bcd_bytes, 3, 1, fp);
    calculate_checksum(fp);
    return true;
}

bool complete_pokedex(FILE *fp)
{
    fseek(fp, POKEDEX_SEEN_OFFSET, SEEK_SET);
    uint8_t buffer[19];
    int len = sizeof(buffer) / sizeof(buffer[0]);

    for(int i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            buffer[i] = 0x7F;
        }
        else
        {
            buffer[i] = 0xFF;
        }
    }

    fwrite(&buffer, sizeof(buffer), 1, fp);

    fseek(fp, POKEDEX_OWNED_OFFSET, SEEK_SET);
    fwrite(&buffer, sizeof(buffer), 1, fp);

    calculate_checksum(fp);
    return true;
}

bool max_item(FILE *fp, uint8_t item)
{
    uint8_t byte = 0;
    uint8_t item_count = 0;
    uint8_t cur_item = 0;
    uint8_t cur_item_count = 0;
    uint8_t max = 0x63;
    uint8_t term = 0xFF;
    long pos = 0;

    fseek(fp, BAG_ITEMS_OFFSET, SEEK_SET);
    fread(&item_count, 1, 1, fp);

    for(int i = 0; i < item_count; i++)
    {
        fread(&cur_item, 1, 1, fp);
        fread(&cur_item_count, 1, 1, fp);

        if(cur_item == item)
        {
            printf("Item [%s] found in bag slot [%i], setting to 99.\n", items[item], i+1);
            pos = ftell(fp) - 1;
            fseek(fp, pos, SEEK_SET);
            fwrite(&max, 1, 1, fp);

            calculate_checksum(fp);
            return true;
        }
    }

    if(item_count < 20)
    {
        printf("Adding item [%s] to bag slot [%i]\n", items[item], item_count+1);
        fwrite(&item, 1, 1, fp);
        fwrite(&max, 1, 1, fp);
        fwrite(&term, 1, 1, fp);
        fseek(fp, BAG_ITEMS_OFFSET, SEEK_SET);
        item_count++;
        fwrite(&item_count, 1, 1, fp);
        calculate_checksum(fp);
        return true;
    }
    else
    {
        printf("Error: Your bag is full, cannnot append another item.\n");
        return false;
    }
}

bool edit_attack_xp(FILE *fp, bool party, int slot, int xp)
{
    if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    pokemon *p = load_pokemon(fp, party, slot);
    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    uint16_t new_attack = (uint16_t)floor(((2 * base->attack + p->attack_iv + floor(sqrt((double)xp) / 4)) * p->level / 100) + 5);
    uint8_t packed[2];
    packed[0] = (new_attack >> 8) & 0xFF;
    packed[1] = new_attack & 0xFF;

    fseek(fp, p->offset_atk, SEEK_SET);
    int write1 = fwrite(&packed, sizeof(packed), 1, fp);
    
    p->atk_stat_exp[0] = hi;
    p->atk_stat_exp[1] = lo;
    fseek(fp, p->offset_atk_stat_exp, SEEK_SET);
    int write2 = fwrite(&p->atk_stat_exp, sizeof(p->atk_stat_exp), 1, fp);

    free(p);
    if(write1 && write2)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

bool edit_def_xp(FILE *fp, bool party, int slot, int xp)
{
   if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    pokemon *p = load_pokemon(fp, party, slot);
    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    uint16_t new_def = (uint16_t)floor(((2 * base->defense + p->defense_iv + floor(sqrt((double)xp) / 4)) * p->level / 100) + 5);
    uint8_t packed[2];
    packed[0] = (new_def >> 8) & 0xFF;
    packed[1] = new_def & 0xFF;
    
    fseek(fp, p->offset_def, SEEK_SET);
    int write1 = fwrite(&packed, sizeof(packed), 1, fp);
    
    p->def_stat_exp[0] = hi;
    p->def_stat_exp[1] = lo;
    fseek(fp, p->offset_def_stat_exp, SEEK_SET);
    int write2 = fwrite(&p->def_stat_exp, sizeof(p->def_stat_exp), 1, fp);

    free(p);
    if(write1 && write2)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

bool edit_speed_xp(FILE *fp, bool party, int slot, int xp)
{
    if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    pokemon *p = load_pokemon(fp, party, slot);
    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    uint16_t new_speed = (uint16_t)floor(((2 * base->speed + p->speed_iv + floor(sqrt((double)xp) / 4)) * p->level / 100) + 5);
    uint8_t packed[2];
    packed[0] = (new_speed >> 8) & 0xFF;
    packed[1] = new_speed & 0xFF;
    
    fseek(fp, p->offset_speed, SEEK_SET);
    int write1 = fwrite(&packed, sizeof(packed), 1, fp);
    
    p->speed_stat_exp[0] = hi;
    p->speed_stat_exp[1] = lo;
    fseek(fp, p->offset_speed_stat_exp, SEEK_SET);
    int write2 = fwrite(&p->speed_stat_exp, sizeof(p->speed_stat_exp), 1, fp);

    free(p);
    if(write1 && write2)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

bool edit_special_xp(FILE *fp, bool party, int slot, int xp)
{
    if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    pokemon *p = load_pokemon(fp, party, slot);
    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    uint16_t new_special = (uint16_t)floor(((2 * base->special + p->special_iv + floor(sqrt((double)xp) / 4)) * p->level / 100) + 5);
    uint8_t packed[2];
    packed[0] = (new_special >> 8) & 0xFF;
    packed[1] = new_special & 0xFF;
    
    fseek(fp, p->offset_special, SEEK_SET);
    int write1 = fwrite(&packed, sizeof(packed), 1, fp);
    
    p->special_stat_exp[0] = hi;
    p->special_stat_exp[1] = lo;
    fseek(fp, p->offset_special_stat_exp, SEEK_SET);
    int write2 = fwrite(&p->special_stat_exp, sizeof(p->special_stat_exp), 1, fp);

    free(p);
    if(write1 && write2)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

bool edit_hp_xp(FILE *fp, bool party, int slot, int xp)
{
    if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    pokemon *p = load_pokemon(fp, party, slot);
    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    uint16_t new_hp = (uint16_t)floor(((2 * base->hp + p->hp_iv + floor(sqrt((double)xp) / 4)) * p->level / 100) + p->level + 10);    
    uint8_t packed[2];
    packed[0] = (new_hp >> 8) & 0xFF;
    packed[1] = new_hp & 0xFF;
    
    fseek(fp, p->offset_max_hp, SEEK_SET);
    int write1 = fwrite(&packed, sizeof(packed), 1, fp);
    
    p->hp_stat_exp[0] = hi;
    p->hp_stat_exp[1] = lo;
    fseek(fp, p->offset_hp_stat_exp, SEEK_SET);
    int write2 = fwrite(&p->hp_stat_exp, sizeof(p->hp_stat_exp), 1, fp);

    free(p);
    if(write1 && write2)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

//above stat xp functions are working however i think the formula is wrong because the end 
//calculated results are off when i deposit and winthdraw a pokemon
//however maxing out the xp stats does work 

bool edit_xp_stat(FILE *fp, int stat_select, bool party, int slot, int xp)
{
    switch (stat_select)
    {
        case 1:
            edit_attack_xp(fp, party, slot, xp);
            break;
        case 2:
            edit_def_xp(fp, party, slot, xp);
            break;
        case 3:
            edit_speed_xp(fp, party, slot, xp);
            break;
        case 4:
            edit_special_xp(fp, party, slot, xp);
            break;
        case 5:
            edit_hp_xp(fp, party, slot, xp);
        case 0:
            edit_attack_xp(fp, party, slot, xp);
            edit_def_xp(fp, party, slot, xp);
            edit_speed_xp(fp, party, slot, xp);
            edit_special_xp(fp, party, slot, xp);
            edit_hp_xp(fp, party, slot, xp);
            break;
        default:
            printf("Not a valid selection, quitting.\n");
            return false;
    }
    return true;
}

bool edit_iv_values(FILE *fp, bool party, int slot, int iv, uint8_t val)
{
    if(val > 0xF)
    {
        printf("Error: Integer overflow, value cannot exceed 15\n");
        exit(1);
    }

    pokemon *p = load_pokemon(fp, party, slot);

    switch (iv)
    {
        case 1:
            p->attack_iv = val;
            break;
        case 2:
            p->defense_iv = val;
            break;
        case 3:
            p->speed_iv = val;
            break;
        case 4:
            p->special_iv = val;
            break;
        case 0:
            p->attack_iv = val;
            p->defense_iv = val;
            p->speed_iv = val;
            p->special_iv = val;
            break;
        default:
            printf("Error: IV/DV unknown, quitting.\n");
            exit(1);
    }
    
    p->iv_data[0] = (p->attack_iv << 4) | p->defense_iv;
    p->iv_data[1] = (p->speed_iv << 4) | p->special_iv;
    
    fseek(fp, p->offset_iv_data, SEEK_SET);
    int w = fwrite(&p->iv_data, sizeof(p->iv_data), 1, fp);
    free(p);
    if(w)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}