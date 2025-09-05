#include <stdio.h>
#include <stdint.h>
#include "editsav.h"
#include "offsets.h"

#define BUFF_SIZE 3979

uint16_t calculate_checksum(FILE *fp)
{
    uint8_t checksum[2];
    fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
    fread(&checksum, sizeof(checksum), 1, fp);
    
    //concat lo and hi bits
    uint16_t stored = checksum[0] | (checksum[1] << 8);

    uint16_t calculated_checksum = 0;
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
    
    if(calculated_checksum == stored)
    {
        //printf("%-15s [%04X]\n", "Save checksum:", calculated_checksum);
        return calculated_checksum;
    }

    return -1;
}