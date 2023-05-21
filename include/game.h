#ifndef RPG_GAME_H
#define RPG_GAME_H

#include <map>
#include <string>

#include "entity.h"
#include "location.h"

extern Player player;
extern Inventory inventory;

extern int render_distance;
extern int center_x;
extern int center_y;
extern std::map<std::pair<int, int>, Chunk> loaded_chunks;

void start_new_game(std::string player_name, std::string password);
void save_game();
void load_game(std::string player_name);

void obtain_loot(int luck);
void explosion(double x, double y, int damage, int range, bool destroy_environment);

#endif