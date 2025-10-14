#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "pkmnstructs.h"
#include "editsav.h"
#include "savinfo.h"
#include "helpers.h"
#include "offsets.h"

void print_usage(const char *program_name) {
    printf("Usage: %s -f <savefile> [OPTIONS]\n\n", program_name);
    printf("Required:\n");
    printf("  -f, --file <file>        Save file to read/edit\n\n");
    
    printf("Summary Mode:\n");
    printf("  -s, --summary            Enable summary mode\n");
    printf("      --show-all           Show complete save summary\n");
    printf("      --player-name        Show player name\n");
    printf("      --player-money       Show player money\n");
    printf("      --player-id          Show player ID\n");
    printf("      --hof-entries        Show Hall of Fame entries\n");
    printf("      --play-time          Show play time\n");
    printf("      --bag-items          Show bag items\n");
    printf("      --party-members      Show party members\n");
    printf("      --player-badges      Show player badges\n");
    printf("      --pokedex            Show Pokedex summary\n");
    printf("      --box-pokemon        Show box Pokemon\n");
    printf("      --pokemon-summary    Show Pokemon summary (requires -p and -l)\n\n");
    
    printf("Edit Mode:\n");
    printf("  -e, --edit               Enable edit mode\n");
    printf("      --money <amount>     Set player money\n");
    printf("      --complete-dex       Complete the Pokedex\n");
    printf("      --max-item <id>      Max out item\n\n");
    
    printf("Pokemon Selection (for Pokemon-specific operations):\n");
    printf("  -p, --pokemon <slot>     Select Pokemon slot (1-6 for party, 1-20 for box)\n");
    printf("  -l, --location <loc>     Location (0=party, 1=box)\n");
    printf("  -b, --box <num>          Box number (1-12, required if location is box)\n\n");
    
    printf("Pokemon Edits (require -e, -p, and -l):\n");
    printf("      --iv <stat> <val>    Edit IV\n");
    printf("                           stat: 6=ATK, 7=DEF, 8=SPD, 9=SPC, 10=ALL\n");
    printf("                           val: 0-15\n");
    printf("      --xp <stat> <val>    Edit XP\n");
    printf("                           stat: 0=ATK, 1=DEF, 2=SPD, 3=SPC, 4=HP, 5=ALL\n");
    printf("                           val: 0-65535\n");
    
    printf("Examples:\n");
    printf("  %s -f save.sav --show-all\n", program_name);
    printf("  %s -f save.sav --party-members\n", program_name);
    printf("  %s -f save.sav -e --money 999999\n", program_name);
    printf("  %s -f save.sav -e -p 1 -l 0 --iv 7 15\n", program_name);
    printf("  %s -f save.sav -s -p 3 -l 1 -b 2 --pokemon-summary\n", program_name);
}

int main(int argc, char *argv[]) {
    char *filename = NULL;
    bool edit_mode = false;
    bool summary_mode = false;
    
    // Summary flags
    bool show_all = false;
    bool show_player_name = false;
    bool show_player_money = false;
    bool show_player_id = false;
    bool show_hof = false;
    bool show_time = false;
    bool show_items = false;
    bool show_party = false;
    bool show_badges = false;
    bool show_dex = false;
    bool show_box = false;
    bool show_pokemon = false;
    
    // Edit values
    uint32_t money_value = 0;
    bool has_money = false;
    bool complete_dex = false;
    uint8_t item_id = 0;
    bool has_item = false;
    
    // Pokemon selection
    int pokemon_slot = -1;
    PokemonLocation pokemon_loc = IN_PARTY;
    int box_num = -1;
    bool has_location = false;
    
    // Pokemon edits
    StatSelection iv_stat = -1, xp_stat = -1, stat_sel = -1;
    int iv_val = -1;
    uint16_t xp_val = 0;
    int stat_val = -1;
    bool has_iv = false, has_xp = false;
    
    static struct option long_options[] = {
        {"file", required_argument, 0, 'f'},
        {"summary", no_argument, 0, 's'},
        {"edit", no_argument, 0, 'e'},
        {"pokemon", required_argument, 0, 'p'},
        {"location", required_argument, 0, 'l'},
        {"box", required_argument, 0, 'b'},
        {"help", no_argument, 0, 'h'},
        
        {"show-all", no_argument, 0, 1000},
        {"player-name", no_argument, 0, 1001},
        {"player-money", no_argument, 0, 1002},
        {"player-id", no_argument, 0, 1003},
        {"hof-entries", no_argument, 0, 1004},
        {"play-time", no_argument, 0, 1005},
        {"bag-items", no_argument, 0, 1006},
        {"party-members", no_argument, 0, 1007},
        {"player-badges", no_argument, 0, 1008},
        {"pokedex", no_argument, 0, 1009},
        {"box-pokemon", no_argument, 0, 1010},
        {"pokemon-summary", no_argument, 0, 1011},
        
        {"money", required_argument, 0, 2001},
        {"complete-dex", no_argument, 0, 2002},
        {"max-item", required_argument, 0, 2003},
        {"iv", required_argument, 0, 2004},
        {"xp", required_argument, 0, 2005},
        {"stat", required_argument, 0, 2006},
        
        {0, 0, 0, 0}
    };
    
    int opt, option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "f:sep:l:b:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 's':
                summary_mode = true;
                break;
            case 'e':
                edit_mode = true;
                break;
            case 'p':
                pokemon_slot = atoi(optarg);
                break;
            case 'l':
                pokemon_loc = (PokemonLocation)atoi(optarg);
                has_location = true;
                break;
            case 'b':
                box_num = atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            
            case 1000: 
                summary_mode = true; 
                show_all = true; 
                break;
            case 1001: summary_mode = true; show_player_name = true; break;
            case 1002: summary_mode = true; show_player_money = true; break;
            case 1003: summary_mode = true; show_player_id = true; break;
            case 1004: summary_mode = true; show_hof = true; break;
            case 1005: summary_mode = true; show_time = true; break;
            case 1006: summary_mode = true; show_items = true; break;
            case 1007: summary_mode = true; show_party = true; break;
            case 1008: summary_mode = true; show_badges = true; break;
            case 1009: summary_mode = true; show_dex = true; break;
            case 1010: summary_mode = true; show_box = true; break;
            case 1011: summary_mode = true; show_pokemon = true; break;
            
            case 2001:
                money_value = atoi(optarg);
                has_money = true;
                break;
            case 2002:
                complete_dex = true;
                break;
            case 2003:
                item_id = atoi(optarg);
                has_item = true;
                break;
            case 2004:  // --iv
                iv_stat = (StatSelection)atoi(optarg);
                if (optind < argc && argv[optind][0] != '-') {
                    iv_val = atoi(argv[optind++]);
                }
                has_iv = true;
                break;
            case 2005:  // --xp
                xp_stat = (StatSelection)atoi(optarg);
                if (optind < argc && argv[optind][0] != '-') {
                    xp_val = atoi(argv[optind++]);
                }
                has_xp = true;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Validate required arguments
    if (!filename) {
        fprintf(stderr, "Error: Save file required (-f)\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Open file in appropriate mode
    const char *mode = edit_mode ? "r+b" : "rb";
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        fprintf(stderr, "Error: Could not open '%s'\n", filename);
        return 1;
    }
    
    // Summary mode
    if (summary_mode) {
        // If --show-all or no specific flags set, show everything
        if (show_all || (!show_player_name && !show_player_money && !show_player_id && 
            !show_hof && !show_time && !show_items && !show_party && 
            !show_badges && !show_dex && !show_box && !show_pokemon)) {
            show_sav_summary(fp);
        } else {
            // Show only what was requested
            if (show_player_name) get_player_name(fp);
            if (show_player_money) get_player_money(fp);
            if (show_player_id) get_player_id(fp);
            if (show_hof) get_hof_entries(fp);
            if (show_time) get_play_time(fp);
            if (show_items) get_bag_items(fp);
            if (show_party) get_party_members(fp);
            if (show_badges) get_player_badges(fp);
            if (show_dex) get_pokedex_summary(fp);
            if (show_box) show_box_pokemon(fp);
            
            if (show_pokemon) {
                // Validate Pokemon selection
                if (pokemon_slot == -1) {
                    fprintf(stderr, "Error: --pokemon-summary requires -p <slot>\n");
                    fclose(fp);
                    return 1;
                }
                
                if (!has_location) {
                    fprintf(stderr, "Error: --pokemon-summary requires -l <location>\n");
                    fclose(fp);
                    return 1;
                }
                
                if (pokemon_loc == IN_BOX && box_num == -1) {
                    fprintf(stderr, "Error: Box Pokemon requires -b <box_num>\n");
                    fclose(fp);
                    return 1;
                }
                
                // Load and display Pokemon
                pokemon *p;
                if (pokemon_loc == IN_PARTY) {
                    uint8_t party_count = 0;
                    fseek(fp, PARTY_OFFSET, SEEK_SET);
                    fread(&party_count, 1, 1, fp);
                    
                    if (pokemon_slot < 1 || pokemon_slot > party_count) {
                        fprintf(stderr, "Error: Invalid party slot %d (party has %d Pokemon)\n", 
                                pokemon_slot, party_count);
                        fclose(fp);
                        return 1;
                    }
                    
                    pokemon *party = load_party_pokemon(fp, party_count);
                    show_pokemon_summary(fp, &party[pokemon_slot - 1], IN_PARTY);
                    free(party);
                } else {
                    // Box Pokemon
                    p = load_pokemon(fp, IN_BOX, box_num, pokemon_slot, 0, 0);
                    if (p) {
                        show_pokemon_summary(fp, p, IN_BOX);
                        free(p);
                    } else {
                        fprintf(stderr, "Error: Could not load Pokemon from box %d slot %d\n", 
                                box_num, pokemon_slot);
                        fclose(fp);
                        return 1;
                    }
                }
            }
        }
    }
    
    // Edit mode
    if (edit_mode) {
        bool changes_made = false;
        
        // Simple edits (don't require Pokemon selection)
        if (has_money) {
            if (edit_money(fp, money_value)) {
                printf("Money set to %u\n", money_value);
                changes_made = true;
            } else {
                fprintf(stderr, "Failed to edit money\n");
            }
        }
        
        if (complete_dex) {
            if (complete_pokedex(fp)) {
                printf("Pokedex completed\n");
                changes_made = true;
            } else {
                fprintf(stderr, "Failed to complete Pokedex\n");
            }
        }
        
        if (has_item) {
            if (max_item(fp, item_id)) {
                printf("Item %u maxed\n", item_id);
                changes_made = true;
            } else {
                fprintf(stderr, "Failed to max item\n");
            }
        }
        
        // Pokemon-specific edits
        if (has_iv || has_xp) {
            // Validate Pokemon selection
            if (pokemon_slot == -1) {
                fprintf(stderr, "Error: Pokemon edits require -p <slot>\n");
                fclose(fp);
                return 1;
            }
            
            if (!has_location) {
                fprintf(stderr, "Error: Pokemon edits require -l <location>\n");
                fclose(fp);
                return 1;
            }
            
            if (pokemon_loc == IN_BOX && box_num == -1) {
                fprintf(stderr, "Error: Box Pokemon edits require -b <box_num>\n");
                fclose(fp);
                return 1;
            }
            
            // Load the Pokemon
            pokemon *p;
            if (pokemon_loc == IN_PARTY) {
                uint8_t party_count = 0;
                fseek(fp, PARTY_OFFSET, SEEK_SET);
                fread(&party_count, 1, 1, fp);
                
                if (pokemon_slot < 1 || pokemon_slot > party_count) {
                    fprintf(stderr, "Error: Invalid party slot %d (party has %d Pokemon)\n", 
                            pokemon_slot, party_count);
                    fclose(fp);
                    return 1;
                }
                
                pokemon *party = load_party_pokemon(fp, party_count);
                p = &party[pokemon_slot - 1];
                
                // Perform edits
                if (has_iv) {
                    if (edit_iv_values(fp, p, iv_stat, iv_val, pokemon_loc)) {
                        printf("IV edited successfully\n");
                        changes_made = true;
                    } else {
                        fprintf(stderr, "Failed to edit IV\n");
                    }
                }
                
                if (has_xp) {
                    if (edit_xp_values(fp, p, xp_stat, pokemon_loc, xp_val)) {
                        printf("XP edited successfully\n");
                        changes_made = true;
                    } else {
                        fprintf(stderr, "Failed to edit XP\n");
                    }
                }
                
                free(party);
            } else {
                // Box Pokemon
                p = load_pokemon(fp, IN_BOX, box_num, pokemon_slot, 0, 0);
                if (!p) {
                    fprintf(stderr, "Error: Could not load Pokemon from box %d slot %d\n", 
                            box_num, pokemon_slot);
                    fclose(fp);
                    return 1;
                }
                
                // Perform edits
                if (has_iv) {
                    if (edit_iv_values(fp, p, iv_stat, iv_val, pokemon_loc)) {
                        printf("IV edited successfully\n");
                        changes_made = true;
                    } else {
                        fprintf(stderr, "Failed to edit IV\n");
                    }
                }
                
                if (has_xp) {
                    if (edit_xp_values(fp, p, xp_stat, pokemon_loc, xp_val)) {
                        printf("XP edited successfully\n");
                        changes_made = true;
                    } else {
                        fprintf(stderr, "Failed to edit XP\n");
                    }
                }
                
                free(p);
            }
        }
        
        if (changes_made) {
            printf("\nAll changes saved to %s\n", filename);
        } else {
            printf("No changes made\n");
        }
    }
    
    fclose(fp);
    return 0;
}