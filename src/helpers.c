#include <stdio.h>
#include <stdlib.h>
#include "pkmnstructs.h"
#include "helpers.h"
#include "offsets.h"

pokemon *load_party_pokemon(FILE *fp, uint8_t party_count)
{
    if(party_count > 6 || party_count < 1)
    {
        printf("Error while reading party size, expected 1-6 got %u\n", party_count);
        exit(1);
    }

    pokemon *party = malloc(party_count * sizeof(pokemon));
    if(!party)
    {
        printf("Error allocating party in memory, quitting.\n");
        exit(1);
    }

    long party_struct = 9;

    for(int i = 0; i < party_count; i++)
    {
        party[i].in_party = true;
        party[i].slot_position = i+1;
        //get current pointer position
        long cur_postition = ftell(fp);
        party[i].offset = cur_postition;

        //read species
        fread(&party[i].species, 1, 1, fp);

        //jump to start of pokemon struct
        fseek(fp, PARTY_OFFSET + party_struct, SEEK_SET);

        //read current hp (2 bytes)
        fread(&party[i].current_hp, 1, 2, fp);

        // skip trash level byte
        fseek(fp, 1, SEEK_CUR);

        //read status condition
        fread(&party[i].status_cond, 1, 1, fp);

        //read pokemon type 1 and 2
        fread(&party[i].type1, 1, 1, fp);
        fread(&party[i].type2, 1, 1, fp);

        //read catch rate/held item
        fread(&party[i].catch_rate_held_item, 1, 1, fp);

        //read moves for pokemon
        fread(&party[i].move1, 1, 1, fp);
        fread(&party[i].move2, 1, 1, fp);
        fread(&party[i].move3, 1, 1, fp);
        fread(&party[i].move4, 1, 1, fp);

        //read OT ID (2 bytes)
        fread(&party[i].orig_trainer_id, 1, 2, fp);

        //read exp points (3 bytes)
        fread(&party[i].exp, 1, 3, fp);

        //read stat exp (hp, attack, defense, speed, special respectivelty)
        fread(&party[i].hp_stat_exp, 1, 2, fp);
        fread(&party[i].atk_stat_exp, 1, 2, fp);
        fread(&party[i].def_stat_exp, 1, 2, fp);
        fread(&party[i].speed_stat_exp, 1, 2, fp);
        fread(&party[i].special_stat_exp, 1, 2, fp);

        //IV data
        fread(&party[i].iv_data, 1, 2, fp);

        //PP values of moves 1-4 respectively
        fread(&party[i].move1_pp, 1, 1, fp);
        fread(&party[i].move2_pp, 1, 1, fp);
        fread(&party[i].move3_pp, 1, 1, fp);
        fread(&party[i].move4_pp, 1, 1, fp);

        //level data
        fread(&party[i].level, 1, 1, fp);

        //Stats (max hp, attack, defense, speed, special respectively)
        fread(&party[i].max_hp, 1, 2, fp);
        fread(&party[i].atk, 1, 2, fp);
        fread(&party[i].def, 1, 2, fp);
        fread(&party[i].speed, 1, 2, fp);
        fread(&party[i].special, 1, 2, fp);
        
        //increment party_struct by 44 bytes to be able to jump to
        //the next pokemons struct
        party_struct += 44;

        //jump back to the curennt position +1 to read the next pokemon
        //species value.
        fseek(fp, cur_postition+1, SEEK_SET);
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