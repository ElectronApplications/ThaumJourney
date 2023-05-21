#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "io.h"
#include "location.h"
#include "loader.h"
#include "block.h"
#include "game.h"
#include "PerlinNoise.hpp"

using namespace std;

Chunk structures[static_cast<int>(StructuresEnum::END)];

void init_structures() {
    ifstream land_file("resources/structures/landstruct.chunk", ios::binary);
    structures[static_cast<int>(StructuresEnum::LandStructure)] = read_chunk(land_file);
    ifstream aqua_file("resources/structures/aquastruct.chunk", ios::binary);
    structures[static_cast<int>(StructuresEnum::AquaticStructure)] = read_chunk(aqua_file);
    ifstream fort_file("resources/structures/fortstruct.chunk", ios::binary);
    structures[static_cast<int>(StructuresEnum::FortressStructure)] = read_chunk(fort_file);
}

void change_render_distance() {
    int temp_render = (int) ceil((double)max(1.5*window_width, 1.5*window_height) / (chunk_size * block_size)) + 3;
    if(temp_render % 2 != 1)
        temp_render++;
    
    if(temp_render != render_distance) {
        render_distance = temp_render;
        load_chunks();
    }
}

void generate_structure(Chunk &chunk) {
    Chunk *structure;
    if(rand() % 3 == 0)
        structure = &structures[static_cast<int>(StructuresEnum::FortressStructure)];
    else if(chunk.blocks[0][0] == static_cast<int>(BlocksEnum::WaterBlock) && chunk.blocks[chunk_size-1][0] == static_cast<int>(BlocksEnum::WaterBlock) && chunk.blocks[0][chunk_size-1] == static_cast<int>(BlocksEnum::WaterBlock) && chunk.blocks[chunk_size-1][chunk_size-1] == static_cast<int>(BlocksEnum::WaterBlock))
        structure = &structures[static_cast<int>(StructuresEnum::AquaticStructure)];
    else
        structure = &structures[static_cast<int>(StructuresEnum::LandStructure)];

    for(int x = 0; x < chunk_size; x++) {
        for(int y = 0; y < chunk_size; y++) {
            if(structure->blocks[y][x] != static_cast<int>(BlocksEnum::AirBlock))
                chunk.blocks[y][x] = structure->blocks[y][x];
                if(rand() % 64 == 0) {
                    if(rand() % 3 == 0)
                        entities.push_back(new Ghost(chunk.x*16 + x, chunk.y*16 + y));
                    else if(blocks[structure->blocks[y][x]]->walkable)
                        entities.push_back(new Skeleton(chunk.x*16 + x, chunk.y*16 + y));
                    else
                        entities.push_back(new Shark(chunk.x*16 + x, chunk.y*16 + y));
                }
        }
    }
}

Chunk generate_chunk(int chunk_x, int chunk_y, int seed) {
    const siv::PerlinNoise::seed_type perlin_seed = static_cast<unsigned int>(seed);
    const siv::PerlinNoise perlin {perlin_seed};

    Chunk chunk {chunk_x, chunk_y};
    for(int x = 0; x < chunk_size; x++) {
        for(int y = 0; y < chunk_size; y++) {
            const double noise = perlin.octave2D_01((chunk_x*chunk_size + x)*0.01, (chunk_y*chunk_size + y)*0.01, 4);
            if(noise < 0.6)
                chunk.blocks[y][x] = static_cast<int>(BlocksEnum::WaterBlock);
            else if(noise < 0.65)
                chunk.blocks[y][x] = static_cast<int>(BlocksEnum::SandBlock);
            else
                chunk.blocks[y][x] = static_cast<int>(BlocksEnum::GrassBlock);
        }
    }

    if(rand() % 35 == 0)
        generate_structure(chunk);

    return chunk;
}

void load_chunks() {
    if(static_cast<int>(player.x) > 0)
        center_x = static_cast<int>(player.x) / chunk_size;
    else
        center_x = -((-static_cast<int>(player.x) + chunk_size-1) / chunk_size);
    
    if(static_cast<int>(player.y) > 0)
        center_y = static_cast<int>(player.y) / chunk_size;
    else
        center_y = -((-static_cast<int>(player.y) + chunk_size-1) / chunk_size);

    for(int x = center_x - render_distance/2; x <= center_x + render_distance/2; x++) {
        for(int y = center_y - render_distance/2; y <= center_y + render_distance/2; y++) {
            if(!loaded_chunks.contains({x, y})) {
                if(filesystem::exists("saves/" + player.player_name + "/chunks/" + to_string(x) + "_" + to_string(y) + ".chunk")) {
                    ifstream file("saves/" + player.player_name + "/chunks/" + to_string(x) + "_" + to_string(y) + ".chunk", ios::binary);
                    loaded_chunks[{x, y}] = read_chunk(file);
                }
                else
                    loaded_chunks[{x, y}] = generate_chunk(x, y, player.seed);
            }
        }
    }
}

pair<int, int> world_to_screen(double x, double y) {
    int screen_x = (window_width - block_size)/2 + (x - player.x - y + player.y) * block_size / 2;
    int screen_y = (window_height - block_size)/2 + (x - player.x + y - player.y) * block_size / 4;
    return make_pair(screen_x, screen_y);
}

pair<double, double> screen_to_world(int x, int y) {
    double shift_x = x - (window_width - block_size)/2;
    double shift_y = y - (window_height - block_size)/2;
    
    double world_x = (shift_x + 2*shift_y)/block_size + player.x - 1;
    double world_y = (-shift_x + 2*shift_y)/block_size + player.y;

    return make_pair(world_x, world_y);
}

void draw_chunk(Chunk &chunk) {
    for(int x = 0; x < chunk_size; x++) {
        for(int y = 0; y < chunk_size; y++) {
            if(blocks[chunk.blocks[y][x]]->texture != nullptr) {
                SDL_Rect dest = {0, 0, block_size, block_size};
                pair<int, int> coords = world_to_screen(x + chunk.x*chunk_size, y + chunk.y*chunk_size);
                dest.x = coords.first;
                dest.y = coords.second;
                SDL_RenderCopy(renderer, blocks[chunk.blocks[y][x]]->texture, nullptr, &dest);
            }
        }
    }
}

int get_block(int x, int y) {
    int chunk_x;
    int block_x = (((x % chunk_size) + chunk_size) % chunk_size);

    if(x >= 0)
        chunk_x = x / chunk_size;
    else
        chunk_x = -((-x + chunk_size-1) / chunk_size);

    int chunk_y;
    int block_y = (((y % chunk_size) + chunk_size) % chunk_size);
    
    if(y >= 0)
        chunk_y = y / chunk_size;
    else
        chunk_y = -((-y + chunk_size-1) / chunk_size);

    if(!loaded_chunks.contains({chunk_x, chunk_y})) {
        if(filesystem::exists("saves/" + player.player_name + "/chunks/" + to_string(chunk_x) + "_" + to_string(chunk_y) + ".chunk")) {
            ifstream file("saves/" + player.player_name + "/chunks/" + to_string(chunk_x) + "_" + to_string(chunk_y) + ".chunk", ios::binary);
            loaded_chunks[{chunk_x, chunk_y}] = read_chunk(file);
        }
        else {
            loaded_chunks[{chunk_x, chunk_y}] = generate_chunk(chunk_x, chunk_y, player.seed);
        }
    }

    return loaded_chunks[{chunk_x, chunk_y}].blocks[block_y][block_x];
}

void set_block(int x, int y, int block) {
    int chunk_x;
    int block_x = (((x % chunk_size) + chunk_size) % chunk_size);

    if(x >= 0)
        chunk_x = x / chunk_size;
    else
        chunk_x = -((-x + chunk_size-1) / chunk_size);

    int chunk_y;
    int block_y = (((y % chunk_size) + chunk_size) % chunk_size);
    
    if(y >= 0)
        chunk_y = y / chunk_size;
    else
        chunk_y = -((-y + chunk_size-1) / chunk_size);

    if(!loaded_chunks.contains({chunk_x, chunk_y})) {
        if(filesystem::exists("saves/" + player.player_name + "/chunks/" + to_string(chunk_x) + "_" + to_string(chunk_y) + ".chunk")) {
            ifstream file("saves/" + player.player_name + "/chunks/" + to_string(chunk_x) + "_" + to_string(chunk_y) + ".chunk", ios::binary);
            loaded_chunks[{chunk_x, chunk_y}] = read_chunk(file);
        }
        else {
            loaded_chunks[{chunk_x, chunk_y}] = generate_chunk(chunk_x, chunk_y, player.seed);
        }
    }

    loaded_chunks[{chunk_x, chunk_y}].blocks[block_y][block_x] = block;
}