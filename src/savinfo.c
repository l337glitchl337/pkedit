#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "species.h"
#include "charset.h"
#include "items.h"
#include "savinfo.h"
#include "offsets.h"
#include "pkmnstructs.h"
#include "helpers.h"

void show_sav_summary(FILE *fp, int mode)
{

   // mode 0 is short-summary
   // mode 1 is long summary

   if(!mode)
   {
    get_player_name(fp);
    get_player_id(fp);
    get_player_money(fp);
    get_hof_entries(fp);
   }
   else
   {
    get_player_name(fp);
    get_player_id(fp);
    get_player_money(fp);
    get_hof_entries(fp);
    get_play_time(fp);
    get_player_badges(fp);
    get_party_members(fp);
    get_bag_items(fp);
    get_pokedex_summary(fp);
   }
}

void get_player_name(FILE *fp)
{
    uint8_t byte = 0;
    fseek(fp, PLAYER_OFFSET, SEEK_SET);
    printf("%-15s ", "Name:");
    printf("[");
    while(true)
    {
        
        fread(&byte, 1, 1, fp);
        //break on terminator
        if(byte == 0x50)
        {
            break;
        }
        else
        {
            printf(gb_charset[byte]);
        }
    }
    printf("]\n");
}

void get_player_money(FILE *fp)
{
    uint8_t money[3];
    fseek(fp, MONEY_OFFSET, SEEK_SET);
    fread(&money, sizeof(money), 1, fp);

    int d0 = (money[0] >> 4) & 0xF;
    int d1 = money[0] & 0xF;
    int d2 = (money[1] >> 4) & 0xF;
    int d3 = money[1] & 0XF;
    int d4 = (money[2] >> 4) & 0xF;
    int d5 = money[2] & 0xF;
    int total = d0*100000 + d1*10000 + d2*1000 + d3*100 + d4*10 + d5;

    printf("%-15s [%i %s]\n", "Money:", total, POKE_YEN);
}

void get_player_id(FILE *fp)
{
    uint8_t id[2];
    uint16_t trainer_id;
    // seek to player id offset and read 2 bytes
    fseek(fp, PLAYER_ID_OFFSET, SEEK_SET);
    fread(&id, sizeof(id), 2, fp);
    //shift hi over 8 bits then concat the low bits.
    trainer_id = (id[0] << 8) | id[1];

    printf("%-15s [%04u]\n", "Trainer ID:", trainer_id);
}

void get_hof_entries(FILE *fp)
{
    uint8_t byte = 0;
    fseek(fp, HALL_OF_FAME_OFFSET, SEEK_SET);
    fread(&byte, 1, 1, fp);
    printf("%-15s [%u]\n", "HoF Entries:", byte);
}

void get_play_time(FILE *fp)
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t max_hours;

    fseek(fp, PLAYTIME_OFFSET, SEEK_SET);
    fread(&hours, 1, 1, fp);
    fread(&max_hours, 1, 1, fp);
    fread(&minutes, 1, 1, fp);

    if(!max_hours)
    {
        printf("%-15s [%u]\n", "PT Hours:", hours);
        printf("%-15s [%u]\n", "PT Mins:", minutes);
    }
    else
    {
        printf("%-15s [%u]\n", "PT Hours:", 255);
        printf("%-15s [%u]\n", "PT Mins:", 59);
    }

    printf("\n");
}

void get_bag_items(FILE *fp)
{
    uint8_t bag_count;
    printf("Bag Items\n");
    fseek(fp, BAG_ITEMS_OFFSET, SEEK_SET);
    fread(&bag_count, 1, 1, fp);

    item *bag_items = load_bag_items(fp, bag_count);

    for(int i = 0; i < bag_count; i++)
    {
        printf("%-15s [%u]\n", items[bag_items[i].item_id], bag_items[i].count);
    }
    free(bag_items);
}

void get_party_members(FILE *fp)
{
    uint8_t party_count = 0;
    fseek(fp, PARTY_OFFSET, SEEK_SET);
    fread(&party_count, 1, 1, fp);

    pokemon *party = load_party_pokemon(fp, party_count);

    for(int i = 0; i < party_count; i++)
    {
        char label[10];
        snprintf(label, sizeof(label), "Slot: %i", i+1);
        printf("%-20s [%s]\n\n", label, party[i].name);
        show_pokemon_summary(fp, &party[i], true);
    }
    free(party);
}

void get_player_badges(FILE *fp)
{
    uint8_t byte = 0;
    printf("Badges\n");
    fseek(fp, BADGE_OFFSET, SEEK_SET);
    fread(&byte, 1, 1, fp);

    const char *badges[] = {"Boulder", "Cascade", "Thunder", "Rainbow", "Soul", "Marsh", "Volcano", "Earth"};
    int len = sizeof(badges) / sizeof(badges[0]);
    int badge_count = 0;

    for (int i = 0; i < len; i++)
    {
        int obtained = (byte >> i) & 1;
        if(obtained)
        {
            badge_count++;
            printf("%-15s [%s]\n", badges[i], "YES");
        }
        else
        {
            printf("%-15s [%s]\n", badges[i], "NO");
        }
    }
    printf("\n");
}

void get_pokedex_summary(FILE *fp)
{
    int owned = 0;
    int seen = 0;
    uint8_t seen_buffer[19];
    uint8_t owned_buffer[19];

    fseek(fp, POKEDEX_SEEN_OFFSET, SEEK_SET);
    fread(&seen_buffer, sizeof(seen_buffer), 1, fp);
    fseek(fp, POKEDEX_OWNED_OFFSET, SEEK_SET);
    fread(&owned_buffer, sizeof(owned_buffer), 1, fp);
    int len = sizeof(seen_buffer) / sizeof(seen_buffer[0]);

    for(int i = 0; i < len; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int s = (seen_buffer[i] >> j) & 1;
            if(s)
            {
                seen++;
            }
        }
    }

    for(int i = 0; i < len; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int s = (owned_buffer[i] >> j) & 1;
            if(s)
            {
                owned++;
            }
        }
    }

    printf("PokeDex Progress\n");
    printf("%-15s [%i]\n", "Seen:", seen);
    printf("%-15s [%i]\n", "Owned", owned);
}

void show_box_pokemon(FILE *fp)
{
    int box_nums = 12;
    int box_size = 0x462;
    char label[10];
    uint8_t current_box_number;
    uint8_t box_count;
    uint8_t box_pokemon;
    uint32_t pos = BOX_OFFSET_1_6;

    fseek(fp, CURRENT_BOX_NUMBER, SEEK_SET);
    fread(&current_box_number, 1 ,1, fp);
    //only concerned with bits 0-7
    current_box_number = current_box_number & 0x07;

    for(int i = 0; i < box_nums; i++)
    {
        if(i == current_box_number)
        {
            fseek(fp, CURRENT_BOX_OFFSET, SEEK_SET);
        }
        else
        {
            fseek(fp, pos, SEEK_SET);
        }
        fread(&box_count, 1, 1, fp);

        if(!box_count)
        {
            snprintf(label, sizeof(label), "Box: %i", i+1);
            printf("%-15s [%s]\n", label, "EMPTY");
        }
        else
        {
            snprintf(label, sizeof(label), "Box: %i", i+1);
            printf("%-15s [%i]\n", label, box_count);
        }

        for(int j = 0; j < box_count; j++)
        {
            //fread(&box_pokemon, 1, 1, fp);
            //printf("└──▶ %-15s\n", species[box_pokemon]);
            pokemon *p = load_pokemon(fp, IN_BOX, i+1, j+1, 0, pos);
            show_pokemon_summary(fp, p, IN_BOX);
            free(p);
        }
        //once we finish the first bank we need to jump to the second bank
        //bank 1 contains boxes 1-6 while bank 2 contains 7-12
        if (i == 5)
        {
            pos = BOX_OFFSET_7_12;
        }
        //go onto the next box
        else
        {
            pos += box_size;
        }
    }
}

void show_pokemon_summary(FILE *fp, pokemon *p, PokemonLocation location)
{
    if(!p->name)
    {
        printf("Error: Unable to load pokemon struct, quitting.\n");
        exit(1);
    }


    if(location != IN_PARTY)
    {
        ExpGroup exp_group;
        uint8_t level = 0;
        for(int i = 0; i < 151; i++)
        {
            if(strcmp(gen1_exp_groups[i].name, p->name) == 0)
            {
                printf("%s\n", gen1_exp_groups[i].name);
                exp_group = gen1_exp_groups[i].group;
                break;
            }
        }

        p->level = get_level_from_exp(p->cal_exp, exp_group);
        const PokemonBaseStats *base = get_base_stats(p->name);
        p->cal_attack = (uint16_t)floor((((base->attack + p->attack_iv) * 2 + floor(sqrt((double)p->cal_atk_xp) / 4)) * p->level / 100) + 5);
        p->cal_defense = (uint16_t)floor((((base->defense + p->defense_iv) * 2 + floor(sqrt((double)p->cal_def_xp) / 4)) * p->level / 100) + 5);
        p->cal_speed = (uint16_t)floor((((base->speed + p->speed_iv) * 2 + floor(sqrt((double)p->cal_speed_xp) / 4)) * p->level / 100) + 5);
        p->cal_special = (uint16_t)floor((((base->special + p->special_iv) * 2 + floor(sqrt((double)p->cal_special_xp) / 4)) * p->level / 100) + 5);

    }

    printf("Stats\n");
    printf("└──▶ %-15s [%u]\n", "Current Level:", p->level);
    printf("└──▶ %-15s [%u]\n", "Current HP:", p->cal_cur_hp);
    printf("└──▶ %-15s [%u]\n", "Attack:", p->cal_attack);
    printf("└──▶ %-15s [%u]\n", "Defense:", p->cal_defense);
    printf("└──▶ %-15s [%u]\n", "Speed:", p->cal_speed);
    printf("└──▶ %-15s [%u]\n\n", "Special:", p->cal_special);

    printf("IV/DVs\n");
    printf("└──▶ %-15s [%u]\n", "HP IV:", p->hp_iv);
    printf("└──▶ %-15s [%u]\n", "Attack IV:", p->attack_iv);
    printf("└──▶ %-15s [%u]\n", "Defense IV:", p->defense_iv);
    printf("└──▶ %-15s [%u]\n", "Speed IV:", p->speed_iv);
    printf("└──▶ %-15s [%u]\n\n", "Special IV:", p->special_iv);


    printf("Moves\n");
    printf("└──▶ %-15s [%s]\n", "Move 1:", moves[p->move1]);
    printf("└──▶ %-15s [%s]\n", "Move 2:", moves[p->move2]);
    printf("└──▶ %-15s [%s]\n", "Move 3:", moves[p->move3]);
    printf("└──▶ %-15s [%s]\n\n", "Move 4:", moves[p->move4]);

    printf("Experience\n");
    printf("└──▶ %-15s [%u]\n", "Exp.:", p->cal_exp);
    printf("└──▶ %-15s [%u]\n", "HP XP:", p->cal_hp_xp);
    printf("└──▶ %-15s [%u]\n", "Atk XP:", p->cal_atk_xp);
    printf("└──▶ %-15s [%u]\n", "Def XP:", p->cal_def_xp);
    printf("└──▶ %-15s [%u]\n", "Speed XP:", p->cal_speed_xp);
    printf("└──▶ %-15s [%u]\n\n", "Special XP:", p->cal_special_xp);

    printf("Type\n");
    printf("└──▶ %-15s [%s]\n", "Type 1:", types[p->type1]);
    printf("└──▶ %-15s [%s]\n\n", "Type 2:", types[p->type2]);
    
    printf("Misc.\n");
    printf("└──▶ %-15s [%s]\n", "Status Con:", status_cond[p->status_cond]);
    printf("└──▶ %-15s [%02X]\n", "Held Item:", p->catch_rate_held_item);
    printf("└──▶ %-15s [%u]\n\n", "OT ID:", p->cal_ot_id);

}