#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>

#include "entity.h"
#include "location.h"
#include "game.h"

using namespace std;

Chunk read_chunk(ifstream &file) {
    Chunk chunk;
    int traits[2];
    file.read(reinterpret_cast<char*>(traits), sizeof(traits));
    chunk.x = traits[0];
    chunk.y = traits[1];
    file.read(reinterpret_cast<char*>(chunk.blocks), sizeof(chunk.blocks));
    return chunk;
}

void write_chunk(ofstream &file, Chunk &chunk) {
    int traits[2] = {chunk.x, chunk.y};
    file.write(reinterpret_cast<char*>(traits), sizeof(traits));
    file.write(reinterpret_cast<char*>(chunk.blocks), sizeof(chunk.blocks));
    file.flush();
}

void write_chunks() {
    filesystem::create_directories("saves/" + player.player_name + "/chunks/");

    for(auto &i : loaded_chunks) {
        ofstream file("saves/" + player.player_name + "/chunks/" + to_string(i.second.x) + "_" + to_string(i.second.y) + ".chunk", ios::binary);
        write_chunk(file, i.second);
    }
}

Entity *read_entity(ifstream &file) {
    EntitiesEnum type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));
    int traits[2];
    file.read(reinterpret_cast<char*>(traits), sizeof(traits));
    double traits_d[2];
    file.read(reinterpret_cast<char*>(traits_d), sizeof(traits_d));

    switch(type) {
        case EntitiesEnum::BasicProjectileEntity: {
            double traits_dp[2];
            file.read(reinterpret_cast<char*>(traits_dp), sizeof(traits_dp));
            int traits_p[2];
            file.read(reinterpret_cast<char*>(traits_p), sizeof(traits_p));
            BasicProjectile *projectile = new BasicProjectile(traits_d[0], traits_d[1], traits_dp[0], traits_dp[1], traits_p[1]);
            projectile->health = traits[0]; 
            projectile->max_health = traits[1];
            projectile->elapsed_time = traits_p[0]; 
            return projectile;
        }
        case EntitiesEnum::SkeletonEntity: {
            int damage_timer;
            file.read(reinterpret_cast<char*>(&damage_timer), sizeof(damage_timer));
            Skeleton *skeleton = new Skeleton(traits_d[0], traits_d[1]);
            skeleton->health = traits[0];
            skeleton->max_health = traits[1];
            skeleton->damage_timer = damage_timer;
            return skeleton;
        }
        case EntitiesEnum::SharkEntity: {
            int damage_timer;
            file.read(reinterpret_cast<char*>(&damage_timer), sizeof(damage_timer));
            Shark *shark = new Shark(traits_d[0], traits_d[1]);
            shark->health = traits[0];
            shark->max_health = traits[1];
            shark->damage_timer = damage_timer;
            return shark;
        }
        case EntitiesEnum::GhostEntity: {
            int damage_timer;
            file.read(reinterpret_cast<char*>(&damage_timer), sizeof(damage_timer));
            Ghost *ghost = new Ghost(traits_d[0], traits_d[1]);
            ghost->health = traits[0];
            ghost->max_health = traits[1];
            ghost->damage_timer = damage_timer;
            return ghost;
        }
        default:
            return new Entity(traits[0], traits[1], traits_d[0], traits_d[1]);
    }
}

void write_entity(ofstream &file, Entity *entity) {
    file.write(reinterpret_cast<char*>(&entity->type), sizeof(entity->type));
    int traits[2] = {entity->health, entity->max_health};
    file.write(reinterpret_cast<char*>(traits), sizeof(traits));
    double traits_d[2] = {entity->x, entity->y};
    file.write(reinterpret_cast<char*>(traits_d), sizeof(traits_d));

    switch(entity->type) {
        case EntitiesEnum::BasicProjectileEntity: {
            BasicProjectile *projectile = dynamic_cast<BasicProjectile*>(entity);
            double traits_dp[2] = {projectile->target_x, projectile->target_y};
            file.write(reinterpret_cast<char*>(traits_dp), sizeof(traits_dp));
            int traits_p[2] = {projectile->elapsed_time, projectile->damage};
            file.write(reinterpret_cast<char*>(traits_p), sizeof(traits_p));
            break;
        }
        case EntitiesEnum::SkeletonEntity: case EntitiesEnum::SharkEntity: case EntitiesEnum::GhostEntity: {
            file.write(reinterpret_cast<char*>(&dynamic_cast<Enemy*>(entity)->damage_timer), sizeof(dynamic_cast<Enemy*>(entity)->damage_timer));
            break;
        }
    }

    file.flush();
}

void write_entities(ofstream &file) {
    size_t amount = entities.size();
    file.write(reinterpret_cast<char*>(&amount), sizeof(amount));
    for(auto i : entities) {
        write_entity(file, i);
    }
}

void read_entities(ifstream &file) {
    size_t amount;
    file.read(reinterpret_cast<char*>(&amount), sizeof(amount));
    for(size_t i = 0; i < amount; i++) {
        entities.push_back(read_entity(file));
    }
}

Player read_player(ifstream &file) {
    Entity *entity_p = read_entity(file);
    Entity entity = *entity_p;
    delete entity_p;

    size_t strlen;
    file.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
    string player_name;
    player_name.resize(strlen);
    file.read(&player_name[0], strlen);
    
    size_t password;
    file.read(reinterpret_cast<char*>(&password), sizeof(password));

    time_t save_time;
    file.read(reinterpret_cast<char*>(&save_time), sizeof(save_time));

    int traits[12];
    file.read(reinterpret_cast<char*>(traits), sizeof(traits));

    return Player(entity, player_name, password, save_time, traits[0], traits[1], traits[2], traits[3], traits[4], traits[5], traits[6], traits[7], traits[8], traits[9], traits[10], traits[11]);
}

void write_player(ofstream &file, Player &player) {
    write_entity(file, &player);

    size_t strlen = player.player_name.size();
    file.write(reinterpret_cast<char*>(&strlen), sizeof(strlen));
    file.write(player.player_name.c_str(), strlen);
    
    file.write(reinterpret_cast<char*>(&player.password), sizeof(player.password));

    file.write(reinterpret_cast<char*>(&player.save_time), sizeof(player.save_time));

    int traits[12] = {player.health_regen, player.mana, player.max_mana, player.mana_regen, player.resistance, player.intelligence, player.dexterity, player.xp, player.level, player.used_points, player.difficulty, player.seed};
    file.write(reinterpret_cast<char*>(traits), sizeof(traits));
    file.flush();
}

vector<Player> load_players() {
    filesystem::create_directory("saves/");

    vector<Player> players_list;
    for(const auto &entry : filesystem::directory_iterator("saves/")) {
        if(entry.is_directory() && filesystem::exists(entry.path().string() + "/player.bin")) {
            ifstream player_file(entry.path().string() + "/player.bin", ios::binary);
            players_list.push_back(read_player(player_file));
        }
    }
    return players_list;
}

Item *read_item(ifstream &file) {
    ItemsEnum type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));

    if(type == ItemsEnum::DefaultFocus) {
        return nullptr;
    }
    else {
        size_t strlen;
        file.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
        string name;
        name.resize(strlen);
        file.read(&name[0], strlen);

        file.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
        string description;
        description.resize(strlen);
        file.read(&description[0], strlen);

        switch(type) {
            case ItemsEnum::HealthPack: {
                int traits[2];
                file.read(reinterpret_cast<char*>(traits), sizeof(traits));
                return new HealthPack(name, description, traits[0], traits[1]);
            }
            case ItemsEnum::ManaPack: {
                int traits[2];
                file.read(reinterpret_cast<char*>(traits), sizeof(traits));
                return new ManaPack(name, description, traits[0], traits[1]);
            }
            case ItemsEnum::BasicFocus: {
                int traits[3];
                file.read(reinterpret_cast<char*>(traits), sizeof(traits));
                return new BasicFocus(name, description, traits[0], traits[1], traits[2]);
            }
            case ItemsEnum::TeleportationFocus: {
                int traits[3];
                file.read(reinterpret_cast<char*>(traits), sizeof(traits));
                return new TeleportationFocus(name, description, traits[0], traits[1], traits[2]);
            }
            case ItemsEnum::ExplosionFocus: {
                int traits[3];
                file.read(reinterpret_cast<char*>(traits), sizeof(traits));
                return new ExplosionFocus(name, description, traits[0], traits[1], traits[2]);
            }
            default:
                return new Item(name, description);
        }
    }
}

void write_item(std::ofstream &file, Item *item) {
    if(item == nullptr) {
        ItemsEnum type = ItemsEnum::DefaultFocus;
        file.write(reinterpret_cast<char*>(&type), sizeof(type));
    }
    else {
        file.write(reinterpret_cast<char*>(&item->type), sizeof(item->type));

        size_t strlen = item->name.size();
        file.write(reinterpret_cast<char*>(&strlen), sizeof(strlen));
        file.write(item->name.c_str(), strlen);
        
        strlen = item->description.size();
        file.write(reinterpret_cast<char*>(&strlen), sizeof(strlen));
        file.write(item->description.c_str(), strlen);

        switch(item->type) {
            case ItemsEnum::HealthPack: {
                HealthPack *pack = dynamic_cast<HealthPack*>(item);
                int traits[2] = {pack->uses_left, pack->regen};
                file.write(reinterpret_cast<char*>(traits), sizeof(traits));
                break;
            }
            case ItemsEnum::ManaPack: {
                ManaPack *pack = dynamic_cast<ManaPack*>(item);
                int traits[2] = {pack->uses_left, pack->regen};
                file.write(reinterpret_cast<char*>(traits), sizeof(traits));
                break;
            }
            case ItemsEnum::BasicFocus: {
                BasicFocus *focus = dynamic_cast<BasicFocus*>(item);
                int traits[3] = {focus->mana_usage, focus->intelligence_requirement, focus->damage};
                file.write(reinterpret_cast<char*>(traits), sizeof(traits));
                break;
            }
            case ItemsEnum::TeleportationFocus: {
                TeleportationFocus *focus = dynamic_cast<TeleportationFocus*>(item);
                int traits[3] = {focus->mana_usage, focus->intelligence_requirement, focus->range};
                file.write(reinterpret_cast<char*>(traits), sizeof(traits));
                break;
            }
            case ItemsEnum::ExplosionFocus: {
                ExplosionFocus *focus = dynamic_cast<ExplosionFocus*>(item);
                int traits[3] = {focus->mana_usage, focus->intelligence_requirement, focus->damage};
                file.write(reinterpret_cast<char*>(traits), sizeof(traits));
                break;
            }
        }
    }
}

void write_inventory(ofstream &file) {
    for(int i = 0; i < 3; i++) {
        write_item(file, inventory.equipped_foci[i]);
    }

    size_t amount = inventory.items.size();
    file.write(reinterpret_cast<char*>(&amount), sizeof(amount));

    for(size_t i = 0; i < amount; i++) {
        write_item(file, inventory.items[i]);
    }
}

void read_inventory(ifstream &file) {
    for(int i = 0; i < 3; i++) {
        inventory.equipped_foci[i] = dynamic_cast<WandFocus*>(read_item(file));
    }

    size_t amount;
    file.read(reinterpret_cast<char*>(&amount), sizeof(amount));

    for(size_t i = 0; i < amount; i++) {
        inventory.items.push_back(read_item(file));
    }
}