#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "editsav.h"
#include "offsets.h"

#define BUFF_SIZE 3979

uint16_t calculate_checksum(FILE *fp)
{
    // 2 byte buffer
/*     uint8_t checksum[2];
    fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
    fread(&checksum, sizeof(checksum), 1, fp); */
    
    //concat lo and hi bits; this is our stored checksum
    //uint16_t stored = checksum[0] | (checksum[1] << 8);

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
    return calculated_checksum;
    
   /*  if(calculated_checksum == stored)
    {
        //printf("%-15s [%04X]\n", "Save checksum:", calculated_checksum);
        return calculated_checksum;
    }

    return -1; */
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

    uint16_t chk = calculate_checksum(fp);
    printf("checksum: %04X\n", chk);
    if(chk > 0)
    {
        fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
        fwrite(&chk, sizeof(chk), 1, fp);
        return true;
    }

    return false;
}