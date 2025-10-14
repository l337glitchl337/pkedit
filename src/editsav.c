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

bool edit_iv_values(FILE *fp, pokemon *p, int iv, int val, int pokemon_location)
{

    if(val > 0xF)
    {
        printf("Error: Integer overflow, value cannot exceed 15\n");
        exit(1);
    }

    switch (iv)
    {
        case ATTACK_IV:
            p->attack_iv = val;
            calc_new_stat(fp, p, ATTACK);
            break;
        case DEFENSE_IV:
            p->defense_iv = val;
            calc_new_stat(fp, p, DEFENSE);
            break;
        case SPEED_IV:
            p->speed_iv = val;
            calc_new_stat(fp, p, SPEED);
            break;
        case SPECIAL_IV:
            p->special_iv = val;
            calc_new_stat(fp, p, SPECIAL);
            break;
        case ALL_IVS:
            p->attack_iv = val;
            p->defense_iv = val;
            p->speed_iv = val;
            p->special_iv = val;
            p->hp_iv = val;
            for(StatSelection stat = ATTACK; stat <= HP; stat++)
            {
                calc_new_stat(fp, p, stat);
            }
            break;
        default:
            printf("Error: IV/DV unknown, quitting.\n");
            exit(1);
    }
    
    p->iv_data[0] = (p->attack_iv << 4) | p->defense_iv;
    p->iv_data[1] = (p->speed_iv << 4) | p->special_iv;
    
    fseek(fp, p->offset_iv_data, SEEK_SET);
    int w = fwrite(&p->iv_data, sizeof(p->iv_data), 1, fp);
    if(w)
    {
        calculate_checksum(fp);
        return true;
    }
    return false;
}

bool edit_xp_values(FILE *fp, pokemon *p, int stat_select, int pokemon_location, uint16_t xp)
{
    if(xp > 0xFFFF)
    {
        printf("Error: Integer overflow, value cannot exceed %u\n", 0xFFFF);
        return false;
    }

    uint16_t val = xp;
    uint8_t lo = val & 0xFF;
    uint8_t hi = (val >> 8) & 0xFF;

    const PokemonBaseStats *base = get_base_stats(p->name);
    if(!base)
    {
        printf("Pokemon not found, quitting.\n");
        return false;
    }

    if(pokemon_location == IN_PARTY)
    {
        switch (stat_select)
        {
            case ATTACK_XP:
                p->cal_atk_xp = val;
                calc_new_stat(fp, p, ATTACK);
                p->atk_stat_exp[0] = hi;
                p->atk_stat_exp[1] = lo;
                fseek(fp, p->offset_atk_stat_exp, SEEK_SET);
                fwrite(&p->atk_stat_exp, sizeof(p->atk_stat_exp), 1, fp);
                break;
            case DEFENSE_XP:
                p->cal_def_xp = val;
                calc_new_stat(fp, p, DEFENSE);
                p->def_stat_exp[0] = hi;
                p->def_stat_exp[1] = lo;
                fseek(fp, p->offset_def_stat_exp, SEEK_SET);
                fwrite(&p->def_stat_exp, sizeof(p->def_stat_exp), 1, fp);
                break;
            case SPEED_XP:
                p->cal_speed_xp = val;
                calc_new_stat(fp, p, SPEED);
                p->speed_stat_exp[0] = hi;
                p->speed_stat_exp[1] = lo;
                fseek(fp, p->offset_speed_stat_exp, SEEK_SET);
                fwrite(&p->speed_stat_exp, sizeof(p->speed_stat_exp), 1, fp);
                break;
            case SPECIAL_XP:
                p->cal_special_xp = val;
                calc_new_stat(fp, p, SPECIAL);
                p->special_stat_exp[0] = hi;
                p->special_stat_exp[1] = lo;
                fseek(fp, p->offset_special_stat_exp, SEEK_SET);
                fwrite(&p->special_stat_exp, sizeof(p->special_stat_exp), 1, fp);
                break;
            case HP_XP:
                p->cal_hp_xp = val;
                calc_new_stat(fp, p, HP);
                p->hp_stat_exp[0] = hi;
                p->hp_stat_exp[1] = lo;
                fseek(fp, p->offset_hp_stat_exp, SEEK_SET);
                fwrite(&p->hp_stat_exp, sizeof(p->hp_stat_exp), 1, fp);
                break;
            case ALL_XPS:
                for(StatSelection stat = ATTACK_XP; stat <= HP_XP; stat++)
                {
                    edit_xp_values(fp, p, stat, pokemon_location, xp);
                }
                break;
            default:
                printf("Error: Invalid stat selection, quitting.\n");
                exit(1);
        }
    }
    else
    {
        switch (stat_select)
        {
            case ATTACK_XP:
                p->atk_stat_exp[0] = hi;
                p->atk_stat_exp[1] = lo;
                fseek(fp, p->offset_atk_stat_exp, SEEK_SET);
                fwrite(p->atk_stat_exp, sizeof(p->atk_stat_exp), 1, fp);
                break;
            case DEFENSE_XP:
                p->def_stat_exp[0] = hi;
                p->def_stat_exp[1] = lo;
                fseek(fp, p->offset_def_stat_exp, SEEK_SET);
                fwrite(p->def_stat_exp, sizeof(p->def_stat_exp), 1, fp);
                break;
            case SPEED_XP:
                p->speed_stat_exp[0] = hi;
                p->speed_stat_exp[1] = lo;
                fseek(fp, p->offset_speed_stat_exp, SEEK_SET);
                fwrite(p->speed_stat_exp, sizeof(p->speed_stat_exp), 1, fp);
                break;
            case SPECIAL_XP:
                p->special_stat_exp[0] = hi;
                p->special_stat_exp[1] = lo;
                fseek(fp, p->offset_special_stat_exp, SEEK_SET);
                fwrite(&p->special_stat_exp, sizeof(p->special_stat_exp), 1, fp);
                break;
            case HP_XP:
                p->hp_stat_exp[0] = hi;
                p->hp_stat_exp[1] = lo;
                fseek(fp, p->offset_hp_stat_exp, SEEK_SET);
                fwrite(&p->hp_stat_exp, sizeof(p->hp_stat_exp), 1, fp);
                break;
            case ALL_XPS:
                //todo
                break;
            default:
                printf("Error: Invalid stat selection, quitting.\n");
                exit(1);

        }
    }

    calculate_checksum(fp);
    return true;
}

bool edit_pokemon(FILE *fp, pokemon *p, int stat_selection, int pokemon_location, int stat_value)
{
    if(stat_selection <= 5)
    {
        edit_xp_values(fp, p, stat_selection, pokemon_location, stat_value);
    }
    else
    {
        edit_iv_values(fp, p, stat_selection, stat_value, pokemon_location);
    }
}