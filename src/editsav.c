#include <stdio.h>
#include <stdint.h>
#include "editsav.h"
#include "offsets.h"

uint16_t calculate_checksum(FILE *fp)
{
    uint8_t checksum[2];
    fseek(fp, CHECKSUM_OFFSET, SEEK_SET);
    fread(&checksum, sizeof(checksum), 1, fp);
    uint16_t stored = checksum[0] | (checksum[1] << 8);

    uint16_t calculated_checksum = 0;
    uint8_t buffer[3979];
    int len = sizeof(buffer) / sizeof(buffer[0]);

    fseek(fp, 0x2598, SEEK_SET);

    fread(&buffer, sizeof(buffer), 1, fp);

    for(int i = 0; i <= len; i++)
    {
        calculated_checksum += buffer[i] & 0xFF;
    }

    calculated_checksum = (~calculated_checksum) & 0xFF;
    
    if(calculated_checksum == stored)
    {
        //printf("%-15s [%04X]\n", "Save checksum:", calculated_checksum);
        return calculated_checksum;
    }

    return -1;
}