#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "savinfo.h"
#include "editsav.h"
#include "helpers.h"
//remove later, for testing
#include "pkmnstructs.h"
#include "species.h"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    
    FILE *fp = fopen(filename, "r+b");

    if(!fp)
    {
        printf("Error opening file '%s', quitting.\n", filename);
        return 1;
    }

/*     pokemon *p = load_pokemon(fp, true, 0, 6, 1);

    for(StatSelection stat = ATTACK_XP; stat <= HP_XP; stat++)
    {
        edit_pokemon(fp, p, stat, IN_PARTY, 65535);
    } */

    get_party_members(fp);
    return 0;

    /* uint16_t chk = calculate_checksum(fp);

    if(chk < 0)
    {
        printf("Checksums do not match, quitting.\n");
        return 1;
    }

    printf("%-15s [%04X]\n", "Save Checksum:", chk);
    long filesize;

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp) / 1024;

    if(filesize > 32)
    {
        printf("Save file is unusually large, quitting.\n");
        return 1;
    }

    printf("%-15s [%d KB]\n", "Save Filesize:", filesize);
    printf("\n");

    if(!fp)
    {
        printf("Error opening file '%s'\n", filename);
        return 1;
    }

    show_sav_summary(filename, fp, 1); */
}