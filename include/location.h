#ifndef RPG_LOCATION_H
#define RPG_LOCATION_H

#include <SDL2/SDL.h>
#include <string>

#include "io.h"

constexpr int chunk_size = 16;

class Chunk {
   public:
    int x;
    int y;
    int blocks[chunk_size][chunk_size];
};

void change_render_distance();

void generate_structure(Chunk &chunk);
Chunk generate_chunk(int chunk_x, int chunk_y, int seed);

std::pair<int, int> world_to_screen(double x, double y);
std::pair<double, double> screen_to_world(int x, int y);

void load_chunks();
void draw_chunk(Chunk &chunk);

int get_block(int x, int y);
void set_block(int x, int y, int block);

enum class StructuresEnum {
    LandStructure,
    AquaticStructure,
    FortressStructure,
    END
};

extern Chunk structures[];
void init_structures();

#endif