#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "io.h"
#include "block.h"

using namespace std;

Block *blocks[static_cast<int>(BlocksEnum::END)];

Block::Block(bool walkable) {
    this->walkable = walkable;
}

Block::Block(bool walkable, string texture_path) {
    this->walkable = walkable;

    SDL_Surface *block_surface = IMG_Load_RW(SDL_RWFromFile(texture_path.c_str(), "r"), 1);
    this->texture = SDL_CreateTextureFromSurface(renderer, block_surface);
    SDL_FreeSurface(block_surface);
}

Block::~Block() {
    if(this->texture != nullptr)
        SDL_DestroyTexture(this->texture);
}

void init_blocks() {
    blocks[static_cast<int>(BlocksEnum::AirBlock)] = new Block(false);
    blocks[static_cast<int>(BlocksEnum::GrassBlock)] = new Block(true, "resources/textures/grass.png");
    blocks[static_cast<int>(BlocksEnum::SandBlock)] = new Block(true, "resources/textures/sand.png");
    blocks[static_cast<int>(BlocksEnum::WaterBlock)] = new Block(false, "resources/textures/water.png");
    blocks[static_cast<int>(BlocksEnum::DirtBlock)] = new Block(true, "resources/textures/dirt.png");
    blocks[static_cast<int>(BlocksEnum::BricksBlock)] = new Block(true, "resources/textures/bricks.png");
    blocks[static_cast<int>(BlocksEnum::CrackedBricksBlock)] = new Block(true, "resources/textures/cracked_bricks.png");
    blocks[static_cast<int>(BlocksEnum::TreasureBricksBlock)] = new Block(true, "resources/textures/treasure_bricks.png");
    blocks[static_cast<int>(BlocksEnum::TrapBricksBlock)] = new Block(true, "resources/textures/trap_bricks.png");
}

void deinit_blocks() {
    for(int i = 0; i < static_cast<int>(BlocksEnum::END); i++)
        delete blocks[i];
}