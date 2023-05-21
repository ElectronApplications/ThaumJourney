#ifndef RPG_BLOCK_H
#define RPG_BLOCK_H

#include <SDL2/SDL.h>
#include <string>

#include "io.h"

enum class BlocksEnum {
    AirBlock,
    GrassBlock,
    SandBlock,
    WaterBlock,
    DirtBlock,
    BricksBlock,
    CrackedBricksBlock,
    TreasureBricksBlock,
    TrapBricksBlock,
    END
};

class Block {
   public:
    SDL_Texture *texture = nullptr;
    bool walkable;
    Block(bool walkable);
    Block(bool walkable, std::string texture_path);
    virtual ~Block();
};

extern Block *blocks[];

void init_blocks();
void deinit_blocks();

#endif