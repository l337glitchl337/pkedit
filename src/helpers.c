#include <stdio.h>
#include <stdlib.h>
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
        pokemon *p = load_pokemon(fp, true, i+1);
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

pokemon *load_pokemon(FILE *fp, bool party, int slot)
{
    uint8_t party_count = 0;
    pokemon *p = malloc(sizeof(pokemon));
    slot -= 1;
    if(!p)
    {
        printf("Error allocating memory for PokeMon struct, quitting.\n");
        exit(1);
    }

    if(party)
    {
        fseek(fp, PARTY_OFFSET, SEEK_SET);
        fread(&party_count, 1, 1, fp);
        
        if(party_count > 6 || party_count < 1)
        {
            printf("Unexpected party count, quitting.\n");
            exit(1);
        }
        long offset = (party_count + (slot * 44)) - slot;

        fseek(fp, slot, SEEK_CUR);
        fread(&p->species, 1 ,1, fp);
        p->name = species[p->species];

        fseek(fp, offset+1, SEEK_CUR);

        fread(&p->current_hp, 1, 2, fp);

        // skip trash level byte
        fseek(fp, 1, SEEK_CUR);

        //read status condition
        fread(&p->status_cond, 1, 1, fp);

        //read pokemon type 1 and 2
        fread(&p->type1, 1, 1, fp);
        fread(&p->type2, 1, 1, fp);

        //read catch rate/held item
        fread(&p->catch_rate_held_item, 1, 1, fp);

        //read moves for pokemon
        fread(&p->move1, 1, 1, fp);
        fread(&p->move2, 1, 1, fp);
        fread(&p->move3, 1, 1, fp);
        fread(&p->move4, 1, 1, fp);

        //read OT ID (2 bytes)
        fread(&p->orig_trainer_id, 1, 2, fp);

        //read exp points (3 bytes)
        fread(&p->exp, 1, 3, fp);

        //read stat exp (hp, attack, defense, speed, special respectivelty)
        fread(&p->hp_stat_exp, 1, 2, fp);
        fread(&p->atk_stat_exp, 1, 2, fp);
        fread(&p->def_stat_exp, 1, 2, fp);
        fread(&p->speed_stat_exp, 1, 2, fp);
        fread(&p->special_stat_exp, 1, 2, fp);

        //IV data
        fread(&p->iv_data, 1, 2, fp);

        //PP values of moves 1-4 respectively
        fread(&p->move1_pp, 1, 1, fp);
        fread(&p->move2_pp, 1, 1, fp);
        fread(&p->move3_pp, 1, 1, fp);
        fread(&p->move4_pp, 1, 1, fp);

        //level data
        fread(&p->level, 1, 1, fp);

        //Stats (max hp, attack, defense, speed, special respectively)
        fread(&p->max_hp, 1, 2, fp);
        fread(&p->atk, 1, 2, fp);
        fread(&p->def, 1, 2, fp);
        fread(&p->speed, 1, 2, fp);
        fread(&p->special, 1, 2, fp);

    }
    else
    {
        //todo
    }

    return p;
}
