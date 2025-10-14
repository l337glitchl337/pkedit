# pkedit

A command-line save editor for Gen 1 Pokémon games (Red/Blue/Yellow). Because sometimes you just want 999999 Pokédollars and perfect IVs.

## What it does

**View stuff:**
- Player info (name, ID, money)
- Party and box Pokémon with full stats
- Bag items, badges, Pokédex progress
- Hall of Fame entries and play time

**Edit stuff:**
- Max out money
- Complete the Pokédex
- Edit Pokémon IVs and stat XP
- Max item quantities

## Building

### Requirements
- CMake 3.10 or higher
- GCC or any C compiler
- Standard C libraries

### Using CMake
```bash
# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Or use make if you prefer
cmake ..
make
```

The binary will be in `build/pkedit`

### Manual compilation (if you really want to)
```bash
gcc -o pkedit src/*.c -Iinclude -lm
```

## Usage

### View your save
```bash
# Everything
pkedit -f save.sav --show-all

# Just party
pkedit -f save.sav --party-members

# Specific Pokémon
pkedit -f save.sav -s -p 1 -l 0 --pokemon-summary
```

### Edit your save
**Always backup your saves first!**

```bash
# Max money
pkedit -f save.sav -e --money 999999

# Complete Pokédex
pkedit -f save.sav -e --complete-dex

# Max out Master Balls (item ID 1)
pkedit -f save.sav -e --max-item 1

# Set party slot 1 Attack IV to 15
pkedit -f save.sav -e -p 1 -l 0 --iv 6 15

# Max all IVs for party slot 1
pkedit -f save.sav -e -p 1 -l 0 --iv 10 15

# Max all stat XP for party slot 1
pkedit -f save.sav -e -p 1 -l 0 --xp 5 65535

# Edit box Pokémon (box 2, slot 5)
pkedit -f save.sav -e -p 5 -l 1 -b 2 --iv 10 15
```

## Quick Reference

### Pokémon Selection
- `-p <slot>` - Slot number (1-6 for party, 1-20 for box)
- `-l <location>` - 0=party, 1=box
- `-b <box>` - Box number 1-12 (only needed for box Pokémon)

### IV Editing (`--iv <stat> <val>`)
- Stats: 6=ATK, 7=DEF, 8=SPD, 9=SPC, 10=ALL
- Values: 0-15

### XP Editing (`--xp <stat> <val>`)
- Stats: 0=ATK, 1=DEF, 2=SPD, 3=SPC, 4=HP, 5=ALL
- Values: 0-65535

## Notes

- Works with .sav and .srm files
- Checksums are automatically recalculated
- HP IV is derived from other IVs (Gen 1 quirk)
- Box Pokémon stats update when you deposit/withdraw them in-game

## Disclaimer

For educational purposes. Use at your own risk. **Backup your saves.**