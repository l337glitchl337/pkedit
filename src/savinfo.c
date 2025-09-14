#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "species.h"
#include "charset.h"
#include "items.h"
#include "savinfo.h"
#include "offsets.h"

uint8_t byte;
uint8_t money[3];
uint8_t id_hi;
uint8_t id_lo;
uint16_t trainer_id;
uint8_t hours;
uint8_t minutes;
uint8_t max_hours;

typedef struct pokemon
{
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

void show_sav_summary(char *filename, FILE *fp, int mode)
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
    byte = 0;
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
    // seek to player id offset and read 2 bytes
    fseek(fp, PLAYER_ID_OFFSET, SEEK_SET);
    fread(&id_hi, 1, 1, fp);
    fread(&id_lo, 1, 1, fp);
    //shift hi over 8 bits then concat the low bits.
    trainer_id = (id_hi << 8) | id_lo;

    printf("%-15s [%04u]\n", "Trainer ID:", trainer_id);
}

void get_hof_entries(FILE *fp)
{
    fseek(fp, HALL_OF_FAME_OFFSET, SEEK_SET);
    fread(&byte, 1, 1, fp);
    printf("%-15s [%u]\n", "HoF Entries:", byte);
}

void get_play_time(FILE *fp)
{
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
    printf("Bag Items\n");
    byte = 0;
    fseek(fp, BAG_ITEMS_OFFSET, SEEK_SET);
    fread(&byte, 1, 1, fp);

    while(true)
    {
        fread(&byte, 1, 1, fp);
        if(byte == BAG_ITEM_TERMINATOR)
        {
            break;
        }
        else
        {
            uint8_t count;
            fread(&count, 1, 1, fp);
            printf("%-15s [%u]\n", items[byte], count);
        }
    }
    printf("\n");
}

void get_party_members(FILE *fp)
{
    uint8_t party_count;
    fseek(fp, PARTY_OFFSET, SEEK_SET);
    fread(&party_count, 1, 1, fp);

    if(party_count > 6 || party_count < 0)
    {
        printf("Error while reading party size, expected 0-6 got %u\n", party_count);
        exit(1);
    }

    pokemon *party[party_count];
    long party_struct = 9;

    for(int i = 0; i < party_count; i++)
    {
        pokemon *new_pokemon = malloc(sizeof(pokemon));
        //get current pointer position
        long cur_postition = ftell(fp);

        if(!new_pokemon)
        {
            printf("Error allocating memory, quitting\n");
            exit(1);
        }
        if(byte == TERMINATOR)
        {
            break;
        }

        //read species
        fread(&new_pokemon->species, 1, 1, fp);

        //jump to start of pokemon struct
        fseek(fp, PARTY_OFFSET + party_struct, SEEK_SET);

        //read current hp (2 bytes)
        fread(&new_pokemon->current_hp, 1, 2, fp);

        // skip trash level byte
        fseek(fp, 1, SEEK_CUR);

        //read status condition
        fread(&new_pokemon->status_cond, 1, 1, fp);

        //read pokemon type 1 and 2
        fread(&new_pokemon->type1, 1, 1, fp);
        fread(&new_pokemon->type2, 1, 1, fp);

        //read catch rate/held item
        fread(&new_pokemon->catch_rate_held_item, 1, 1, fp);

        //read moves for pokemon
        fread(&new_pokemon->move1, 1, 1, fp);
        fread(&new_pokemon->move2, 1, 1, fp);
        fread(&new_pokemon->move3, 1, 1, fp);
        fread(&new_pokemon->move4, 1, 1, fp);

        //read OT ID (2 bytes)
        fread(&new_pokemon->orig_trainer_id, 1, 2, fp);

        //read exp points (3 bytes)
        fread(&new_pokemon->exp, 1, 3, fp);

        //read stat exp (hp, attack, defense, speed, special respectivelty)
        fread(&new_pokemon->hp_stat_exp, 1, 2, fp);
        fread(&new_pokemon->atk_stat_exp, 1, 2, fp);
        fread(&new_pokemon->def_stat_exp, 1, 2, fp);
        fread(&new_pokemon->speed_stat_exp, 1, 2, fp);
        fread(&new_pokemon->special_stat_exp, 1, 2, fp);

        //IV data
        fread(&new_pokemon->iv_data, 1, 2, fp);

        //PP values of moves 1-4 respectively
        fread(&new_pokemon->move1_pp, 1, 1, fp);
        fread(&new_pokemon->move2_pp, 1, 1, fp);
        fread(&new_pokemon->move3_pp, 1, 1, fp);
        fread(&new_pokemon->move4_pp, 1, 1, fp);

        //level data
        fread(&new_pokemon->level, 1, 1, fp);

        //Stats (max hp, attack, defense, speed, special respectively)
        fread(&new_pokemon->max_hp, 1, 2, fp);
        fread(&new_pokemon->atk, 1, 2, fp);
        fread(&new_pokemon->def, 1, 2, fp);
        fread(&new_pokemon->speed, 1, 2, fp);
        fread(&new_pokemon->special, 1, 2, fp);
        
        //increment party_struct by 44 bytes to be able to jump to
        //the next pokemons struct
        party_struct += 44;

        //jump back to the curennt position +1 to read the next pokemon
        //species value.
        fseek(fp, cur_postition+1, SEEK_SET);

        //store new pokemon struct in the party array
        party[i] = new_pokemon;
    }

    for(int i = 0; i < party_count; i++)
    {
        char label[10];
        snprintf(label, sizeof(label), "Slot %i", i + 1);
        printf("%-20s [%s]\n", label, species[party[i]->species]);
        
        // misc data from the pokemon
        // some neccisary bit packing to interpert the values correctly
        // it's primarily packing 2 bytes into a 16 bit integer by shifting and OR'ing
        uint16_t cur_hp = (party[i]->current_hp[0] << 8) | party[i]->current_hp[1];
        uint16_t ot_id = (party[i]->orig_trainer_id[0] << 8) | party[i]->orig_trainer_id[1];
        uint32_t exp = (party[i]->exp[0] << 16) | (party[i]->exp[1] << 8) | party[i]->exp[3];
        uint16_t hp_xp = (party[i]->hp_stat_exp[0] << 8) | party[i]->hp_stat_exp[1];
        uint16_t atk_xp = (party[i]->atk_stat_exp[0] << 8) | party[i]->atk_stat_exp[1];
        uint16_t def_xp = (party[i]->def_stat_exp[0] << 8) | party[i]->def_stat_exp[1];
        uint16_t speed_xp = (party[i]->speed_stat_exp[0] << 8) | party[i]->speed_stat_exp[1];
        uint16_t special_xp = (party[i]->special_stat_exp[0] << 8) | party[i]->special_stat_exp[1];
        uint16_t max_hp = (party[i]->max_hp[0] << 8) | party[i]->max_hp[1];
        uint16_t iv_data = (party[i]->iv_data[0] << 8) | party[i]->iv_data[1];

        //stats from the pokemon
        uint16_t attack = (party[i]->atk[0] << 8) | party[i]->atk[1];
        uint16_t defense = (party[i]->def[0] << 8) | party[i]->def[1];
        uint16_t speed = (party[i]->speed[0] << 8) | party[i]->speed[1];
        uint16_t special = (party[i]->special[0] << 8) | party[i]->special[1];

        //calculating IV's byt doing some bit shifting and masking
        uint16_t attack_iv = iv_data & 0xF;
        uint16_t defense_iv = (iv_data >> 4) & 0xF;
        uint16_t speed_iv = (iv_data >> 8) & 0xF;
        uint16_t special_iv = (iv_data >> 12) & 0xF;

        //last IV is a bit of a pain in the arse to calculate
        //per the documentation, we need to take the LSB of each
        //IV calculated above, and then assemple it into a "binary string"
        //what im doing here is getting the LSB from each IV, then shifting them all over to pack them into 8 bits
        uint8_t hp_iv = (attack_iv & 0x01) << 0 | (defense_iv & 0x01) << 1 | (speed_iv & 0x01) << 2 | (special_iv & 0x01) << 3;

        printf("Stats\n");
        printf("└──▶ %-15s [%u]\n", "Current Level:", party[i]->level);
        printf("└──▶ %-15s [%u/%u]\n", "Current HP:", cur_hp, max_hp);
        printf("└──▶ %-15s [%u]\n", "Attack:", attack);
        printf("└──▶ %-15s [%u]\n", "Defense:", defense);
        printf("└──▶ %-15s [%u]\n", "Speed:", speed);
        printf("└──▶ %-15s [%u]\n\n", "Special:", special);

        printf("IV/DVs\n");
        printf("└──▶ %-15s [%u]\n", "HP IV:", hp_iv);
        printf("└──▶ %-15s [%u]\n", "Attack IV:", attack_iv);
        printf("└──▶ %-15s [%u]\n", "Defense IV:", defense_iv);
        printf("└──▶ %-15s [%u]\n", "Speed IV:", speed_iv);
        printf("└──▶ %-15s [%u]\n\n", "Special IV:", special_iv);

        printf("Moves\n");
        printf("└──▶ %-15s [%s]\n", "Move 1:", moves[party[i]->move1]);
        printf("└──▶ %-15s [%s]\n", "Move 2:", moves[party[i]->move2]);
        printf("└──▶ %-15s [%s]\n", "Move 3:", moves[party[i]->move3]);
        printf("└──▶ %-15s [%s]\n\n", "Move 4:", moves[party[i]->move4]);

        printf("Experience\n");
        printf("└──▶ %-15s [%u]\n", "Exp.:", exp);
        printf("└──▶ %-15s [%u]\n", "HP XP:", hp_xp);
        printf("└──▶ %-15s [%u]\n", "Atk XP:", atk_xp);
        printf("└──▶ %-15s [%u]\n", "Def XP:", def_xp);
        printf("└──▶ %-15s [%u]\n", "Speed XP:", speed_xp);
        printf("└──▶ %-15s [%u]\n\n", "Special XP:", special_xp);

        printf("Type\n");
        printf("└──▶ %-15s [%s]\n", "Type 1:", types[party[i]->type1]);
        printf("└──▶ %-15s [%s]\n\n", "Type 2:", types[party[i]->type2]);
        
        printf("Misc.\n");
        printf("└──▶ %-15s [%s]\n", "Status Con:", status_cond[party[i]->status_cond]);
        printf("└──▶ %-15s [%02X]\n", "Held Item:", party[i]->catch_rate_held_item);
        printf("└──▶ %-15s [%u]\n", "OT ID:", ot_id);
    }
}

void get_player_badges(FILE *fp)
{
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