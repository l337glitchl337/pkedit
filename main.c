#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "savinfo.h"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");

    if(!fp)
    {
        printf("Error opening file '%s'\n", filename);
        return 1;
    }

    show_sav_summary(filename, fp, 1);
}