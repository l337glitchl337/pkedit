#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "species.h"
#include "charset.h"
#include "items.h"
#include "savinfo.h"
#include "offsets.h"

uint8_t party_count;
uint8_t byte;
uint8_t money[3];
uint8_t id_hi;
uint8_t id_lo;
uint16_t trainer_id;
uint8_t hours;
uint8_t minutes;
uint8_t max_hours;

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

    //get hi and low bits (ones place, 10ths place)
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
        //printf("Play time: %u hours and %u minutes\n", hours, minutes);
    }
    else
    {
        printf("%-15s [%u]\n", "PT Hours:", 255);
        printf("%-15s [%u]\n", "PT Mins:", 59);
        //printf("Play time: 255 hours and 59 minutes\n");
    }

    printf("\n");
}

void get_bag_items(FILE *fp)
{
    printf("Bag Items\n");
    byte = 0;
    fseek(fp, BAG_ITEMS_OFFSET, SEEK_SET);
    fread(&byte, 1, 1, fp);
    /* printf("Bag Items (%u/20):\n", byte);
    printf("%-15s %s\n", "Item", "Count");
    printf("%-15s %s\n", "----", "-----"); */

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
    printf("Party Pokemon\n");
    byte = 0;
    fseek(fp, PARTY_OFFSET, SEEK_SET);
    fread(&party_count, 1, 1, fp);
    uint8_t party_pkmn[party_count];

    if(party_count > 6)
    {
        printf("Error while reading party size, expected 0-6 got %u\n", party_count);
        exit(1);
    }

    int count = 0;
    while(byte != TERMINATOR)
    {
        fread(&byte, 1, 1, fp);
        party_pkmn[count] = byte;
        count++;
    }

    for(int i = 0; i < party_count; i++)
    {
        char label[10];
        snprintf(label, sizeof(label), "Slot %i", i + 1);
        printf("%-15s [%s]\n", label, species[party_pkmn[i]]);
    }
    printf("\n");

    uint8_t buffer = 0;
    uint8_t hp_bytes[2];
    uint8_t cur_pkmn;
    const char *status_cond[0x41] = {
        [0x04] = "Asleep",
        [0x08] = "Poisoned",
        [0x10] = "Burned",
        [0x20] = "Frozen",
        [0x40] = "Paralyzed",
        [0x00] = "None"
    
    };

    const char *types[0x1B] = {
        [0x00] = "Normal",
        [0x01] = "Fighting",
        [0x02] = "Flying",
        [0x03] = "Poison",
        [0x04] = "Ground",
        [0x05] = "Rock",
        [0x06] = "Bird", // not used
        [0x07] = "Bug",
        [0x08] = "Ghost",
        [0x14] = "Fire",
        [0x15] = "Water",
        [0x16] = "Grass",
        [0x17] = "Electric",
        [0x18] = "Psychic",
        [0x19] = "Ice",
        [0x1A] = "Dragon"
    };

    long pos = PARTY_OFFSET + 8;
    fseek(fp, pos, SEEK_SET);
    fread(&cur_pkmn, 1, 1, fp);
    fread(&hp_bytes, 1, 2, fp);

    uint16_t cur_hp = (hp_bytes[0] << 8) | hp_bytes[1];
    printf("%-15s\n", species[cur_pkmn]);
    printf("%-15s [%u]\n", "Current HP:", cur_hp);

    pos = ftell(fp) + 1;
    fseek(fp, pos, SEEK_SET);
    fread(&buffer, 1, 1, fp);
    printf("%-15s [%s]\n", "Status Cond.:", status_cond[buffer]);

    fread(&buffer, 1, 1, fp);
    printf("%-15s [%s]\n", "Type 1:", types[buffer]);
    fread(&buffer, 1, 1, fp);
    printf("%-15s [%s]\n", "Type 2:", types[buffer]);
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