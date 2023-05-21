#ifndef RPG_LOADER_H
#define RPG_LOADER_H

#include <vector>
#include <fstream>

#include "entity.h"
#include "location.h"

Chunk read_chunk(std::ifstream &file);
void write_chunk(std::ofstream &file, Chunk &chunk);
void write_chunks();

Entity *read_entity(std::ifstream &file);
void write_entity(std::ofstream &file, Entity *entity);
void write_entities(std::ofstream &file);
void read_entities(std::ifstream &file);

Player read_player(std::ifstream &file);
void write_player(std::ofstream &file, Player &player);
std::vector<Player> load_players();

Item *read_item(std::ifstream &file);
void write_item(std::ofstream &file, Item *item);
void write_inventory(std::ofstream &file);
void read_inventory(std::ifstream &file);

#endif