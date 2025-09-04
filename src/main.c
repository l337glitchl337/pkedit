#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "savinfo.h"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    long filesize;

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp) / 1024;

    if(filesize > 100)
    {
        printf("Save file is unusually large, quitting.\n");
        return 1;
    }

    printf("%-15s [%ld KB]\n", "Save Filesize:", filesize);

    if(!fp)
    {
        printf("Error opening file '%s'\n", filename);
        return 1;
    }

    show_sav_summary(filename, fp, 1);
}