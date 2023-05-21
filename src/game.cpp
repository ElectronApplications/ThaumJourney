#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <cmath>

#include "entity.h"
#include "game.h"
#include "location.h"
#include "block.h"
#include "loader.h"
#include "scene.h"
#include "io.h"

using namespace std;

Player player;
Inventory inventory;

int render_distance = 5;
int center_x = 0;
int center_y = 0;
map<pair<int, int>, Chunk> loaded_chunks;

void start_new_game(string player_name, string password) {
    player.player_name = player_name;
    player.password = hash<string>{}(password);
    
    player.seed = rand();

    player.health = 10;
    player.max_health = 10;
    player.health_regen = 1;
    player.mana = 5;
    player.max_mana = 5;
    player.mana_regen = 1;
    player.resistance = 1;
    player.intelligence = 0;
    player.dexterity = 2;
    player.xp = 0;
    player.level = 1;
    player.used_points = 0;
}

void save_game() {
    time_t current_time = chrono::system_clock::to_time_t(chrono::system_clock().now());
    player.save_time = current_time;

    write_chunks();
    ofstream player_file("saves/" + player.player_name + "/player.bin", ios::binary);
    write_player(player_file, player);
    ofstream entities_file("saves/" + player.player_name + "/entities.bin", ios::binary);
    write_entities(entities_file);
    ofstream inventory_file("saves/" + player.player_name + "/inventory.bin", ios::binary);
    write_inventory(inventory_file);
}

void load_game(string player_name) {
    ifstream player_file("saves/" + player.player_name + "/player.bin", ios::binary);
    player = read_player(player_file);
    ifstream entities_file("saves/" + player.player_name + "/entities.bin", ios::binary);
    read_entities(entities_file);
    ifstream inventory_file("saves/" + player.player_name + "/inventory.bin", ios::binary);
    read_inventory(inventory_file);
}

void obtain_loot(int luck) {
    Item *item;
    int picked_item = rand() % 5;
    switch(picked_item) {
        case 0: {
            int uses = rand() % 3 + 1;
            int regen = rand() % min(player.max_health, luck) + 1;
            item = new HealthPack("Зелье исцеления", "Восстанавливает " + to_string(regen) + " здоровья\nИспользований осталось: " + to_string(uses), uses, regen);
            break;
        }
        case 1: {
            int uses = rand() % 3 + 1;
            int regen = rand() % min(player.max_mana, luck) + 1;
            item = new ManaPack("Зелье восстановления маны", "Восстанавливает " + to_string(regen) + " маны\nИспользований осталось: " + to_string(uses), uses, regen);
            break;
        }
        case 2: {
            int mana_usage = rand() % max(1, min(player.max_mana, player.max_mana -  luck/10)) + 1;
            int intelligence_requirement = rand() % max(2, (player.intelligence - luck/10)) + player.intelligence;
            int damage = rand() % (player.level + luck)/4 + 3;

            item = new BasicFocus("Обычная линза", "Стреляет магическими минами, которые при соприкосновении с врагами, наносят им " + to_string(damage) + " единицы урона.", mana_usage, intelligence_requirement, damage);
            break;
        }
        case 3: {
            int mana_usage = rand() % max(1, min(player.max_mana, player.max_mana -  luck/10)) + 1 + player.max_mana/4;
            int intelligence_requirement = rand() % max(2, (player.intelligence - luck/10)) + player.intelligence + 1;
            int damage = rand() % (player.level + luck) + 5;

            item = new ExplosionFocus("Взрывная линза", "Позволяет создать взрыв в указанной точке, наносящий врагам " + to_string(damage) + " единицы урона. Не наносит урон призракам.", mana_usage, intelligence_requirement, damage);
            break;
        }
        case 4: {
            int mana_usage = rand() % max(1, min(player.max_mana, player.max_mana -  luck/10)) + 1;
            int intelligence_requirement = rand() % max(2, (player.intelligence - luck/10)) + player.intelligence;
            int range = rand() % (player.level + luck) + 32;

            item = new TeleportationFocus("Линза телепортации", "Позволяет телепортироваться в указанную точку.\nРадиус действия: " + to_string(range), mana_usage, intelligence_requirement, range);
            break;
        }
    }

    inventory.items.push_back(item);
    dynamic_cast<Label*>(gui_elements[3])->text += "+ " + item->name + "\n";
    gui_elements[3]->token = 5*fps;
    gui_elements[3]->update();
}

void explosion(double x, double y, int damage, int range, bool destroy_environment) {
    for(auto i : entities) {
        if(dynamic_cast<Enemy*>(i) && pow(x - i->x, 2) + pow(y - i->y, 2) <= range*range && !(dynamic_cast<Ghost*>(i) && destroy_environment)) { 
            i->health -= damage;
            i->damaged = fps;
            dynamic_cast<Enemy*>(i)->vel_x = 5*(i->x - x);
            dynamic_cast<Enemy*>(i)->vel_y = 5*(i->y - y);
        }
    }

    if(pow(x - player.x, 2) + pow(y - player.y, 2) <= range*range) {
        player.health -= ceil((double)damage/player.resistance);
        player.damaged = fps;
        player.vel_x = 5*(player.x - x);
        player.vel_y = 5*(player.y - y);
        update_scene(false);
    }

    if(destroy_environment) {
        for(int block_x = round(x) - range; block_x <= round(x) + range; block_x++) {
            for(int block_y = round(y) - range; block_y <= round(y) + range; block_y++) {
                BlocksEnum block = static_cast<BlocksEnum>(get_block(block_x, block_y));
                switch(block) {
                    case BlocksEnum::BricksBlock: case BlocksEnum::TreasureBricksBlock:
                        set_block(block_x, block_y, static_cast<int>(BlocksEnum::CrackedBricksBlock));
                        break;
                    case BlocksEnum::GrassBlock:
                        set_block(block_x, block_y, static_cast<int>(BlocksEnum::DirtBlock));
                        break;
                }
            }
        }
    }
}