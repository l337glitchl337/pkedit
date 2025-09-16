#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
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
