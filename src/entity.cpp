#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.h"
#include "effect.h"
#include "location.h"
#include "io.h"
#include "game.h"
#include "block.h"
#include "scene.h"

using namespace std;

vector<Entity*> entities;

int spawn_rate = 10;
void Entity::draw() {
    this->sprite->draw_sprite(this->x, this->y, this->sprite_state_type, this->sprite_state/10, this->flip, this->damaged);
    if(dynamic_cast<Enemy*>(this)) {
        pair<int, int> coords = world_to_screen(this->x, this->y);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect dest = {coords.first, coords.second - block_size, block_size, block_size/2};
        SDL_RenderDrawRect(renderer, &dest);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        dest = {coords.first + 2, coords.second - block_size + 2, static_cast<int>((double)block_size * this->health / this->max_health - 4), block_size/2 - 4};
        SDL_RenderFillRect(renderer, &dest);
    }
}

Entity::Entity(int health, int max_health, double x, double y) {
    this->health = health;
    this->max_health = max_health;
    this->x = x;
    this->y = y;
}

Player::Player(Entity &entity, string player_name, size_t password, time_t save_time, int health_regen, int mana, int max_mana, int mana_regen, int resistance, int intelligence, int dexterity, int xp, int level, int used_points, int difficulty, int seed)
: Entity(entity.health, entity.max_health, entity.x, entity.y) {
    this->player_name = player_name;
    this->password = password;
    this->save_time = save_time;
    this->health_regen = health_regen;
    this->mana = mana;
    this->max_mana = max_mana;
    this->mana_regen = mana_regen;
    this->resistance = resistance;
    this->intelligence = intelligence;
    this->dexterity = dexterity;
    this->xp = xp;
    this->level = level;
    this->used_points = used_points;
    this->difficulty = difficulty;
    this->seed = seed;
    this->sprite = sprites[static_cast<int>(SpritesEnum::PlayerSprite)];
}

void Player::action() {
    if(wasd[0] || wasd[1] || wasd[2] || wasd[3] || !blocks[get_block(round(this->x), round(this->y))]->walkable) {
        if(wasd[0]) {
            this->x -= this->dexterity/fps;
            this->y -= this->dexterity/fps;
        }
        if(wasd[1]) {
            this->x -= this->dexterity/fps;
            this->y += this->dexterity/fps;
            this->flip = true;
        }
        if(wasd[2]) {
            this->x += this->dexterity/fps;
            this->y += this->dexterity/fps;
        }
        if(wasd[3]) {
            this->x += this->dexterity/fps;
            this->y -= this->dexterity/fps;
            this->flip = false;
        }
        if(this->sprite_state_type != 1) {
            this->sprite_state_type = 1;
            this->sprite_state = 0;
        }
        else
            this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[1]);
    }
    else {
        if(this->sprite_state_type != 0) {
            this->sprite_state_type = 0;
            this->sprite_state = 0;
        }
        else
            this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[0]);
    }

    this->x += vel_x/fps;
    this->y += vel_y/fps;
    this->vel_x /= pow(10, 1/fps);
    this->vel_y /= pow(10, 1/fps);

    if(this->damaged > 0)
        this->damaged--;
    
    if(this->xp >= this->level*this->level) {
        this->level++;
        update_scene(false);
    }
    
    if(regen_timer + 1 >= static_cast<int>(fps*5)) {
        if(this->health > 0 && this->health < this->max_health) {
            this->health = min(this->max_health, this->health + this->health_regen);
            this->regen_timer = 0;
        }
        if(this->mana < this->max_mana) {
            this->mana = min(this->max_mana, this->mana + this->mana_regen);
            this->regen_timer = 0;
        }
        if(this->regen_timer == 0)
            update_scene(false);
    }
    this->regen_timer = (this->regen_timer + 1) % static_cast<int>(fps*5);

    if(get_block(round(this->x), round(this->y)) == static_cast<int>(BlocksEnum::TrapBricksBlock)) {
        effects.push_back(new Effect(round(this->x), round(this->y), fps, sprites[static_cast<int>(SpritesEnum::BombExplosionSprite)]));
        set_block(round(this->x), round(this->y), static_cast<int>(BlocksEnum::CrackedBricksBlock));
        explosion(round(this->x), round(this->y), 50, 5, true);
    }
    else if(get_block(round(this->x), round(this->y)) == static_cast<int>(BlocksEnum::TreasureBricksBlock)) {
        set_block(round(this->x), round(this->y), static_cast<int>(BlocksEnum::BricksBlock));
        obtain_loot(player.level*10);
    }
}

void Enemy::action() {
    if(pow(this->x - player.x, 2) + pow(this->y - player.y, 2) <= 32*32) {
        double temp_x = this->x + 2*(player.x - this->x > 0 ? 1 : -1)/fps;
        double temp_y = this->y + 2*(player.y - this->y > 0 ? 1 : -1)/fps;

        bool walkable = blocks[get_block(round(temp_x), round(temp_y))]->walkable;
        if((walkable && this->walks || !walkable && this->swims) && pow(this->x - player.x, 2) + pow(this->y - player.y, 2) > 0.25) {
            pair<int, int> coords = world_to_screen(this->x, this->y);
            if(coords.first > window_width/2)
                this->flip = true;
            else
                this->flip = false;
            
            this->x = temp_x;
            this->y = temp_y;

            if(this->sprite->states.size() > 1) {
                if(this->sprite_state_type != 1) {
                    this->sprite_state_type = 1;
                    this->sprite_state = 0;
                }
                else
                    this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[1]);
                }
            }
    }
    else if(this->sprite->states.size() > 1) {
        if(this->sprite_state_type != 0) {
            this->sprite_state_type = 0;
            this->sprite_state = 0;
        }
        else
            this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[0]);
    }

    if(this->sprite->states.size() == 1)
        this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[0]);
    
    this->x += vel_x/fps;
    this->y += vel_y/fps;
    this->vel_x /= pow(10, 1/fps);
    this->vel_y /= pow(10, 1/fps);

    if(this->damaged > 0)
        this->damaged--;
    
    if(this->damage_timer > 0)
        this->damage_timer--;
    
    if(get_block(round(this->x), round(this->y)) == static_cast<int>(BlocksEnum::TrapBricksBlock)) {
        effects.push_back(new Effect(round(this->x), round(this->y), fps, sprites[static_cast<int>(SpritesEnum::BombExplosionSprite)]));
        set_block(round(this->x), round(this->y), static_cast<int>(BlocksEnum::CrackedBricksBlock));
        explosion(round(this->x), round(this->y), 50, 5, true);
    }
}

Skeleton::Skeleton(double x, double y) : Enemy(3*player.difficulty*player.level, x, y) {
    this->sprite = sprites[static_cast<int>(SpritesEnum::SkeletonSprite)];
    this->type = EntitiesEnum::SkeletonEntity;
}

void Skeleton::action() {
    Enemy::action();
    
    if(pow(this->x - player.x, 2) + pow(this->y - player.y, 2) <= 1 && this->damage_timer == 0) {
        player.health -= ceil(0.7*player.difficulty*player.level/player.resistance);
        player.damaged = fps;
        player.vel_x = 5*(player.x - this->x);
        player.vel_y = 5*(player.y - this->y);
        update_scene(false);
        this->damage_timer = 2*fps;
    }
}

Shark::Shark(double x, double y) : Enemy(7*player.difficulty*player.level, x, y) {
    this->sprite = sprites[static_cast<int>(SpritesEnum::SharkSprite)];
    this->type = EntitiesEnum::SharkEntity;
    this->walks = false;
    this->swims = true;
}

void Shark::action() {
    Enemy::action();
    
    if(pow(this->x - player.x, 2) + pow(this->y - player.y, 2) <= 1 && this->damage_timer == 0) {
        player.health -= ceil(1.5*player.difficulty*player.level/player.resistance);
        player.damaged = fps;
        player.vel_x = 5*(player.x - this->x);
        player.vel_y = 5*(player.y - this->y);
        update_scene(false);
        this->damage_timer = 5*fps;
    }
}

Ghost::Ghost(double x, double y) : Enemy(player.difficulty*player.level, x, y) {
    this->sprite = sprites[static_cast<int>(SpritesEnum::GhostSprite)];
    this->type = EntitiesEnum::GhostEntity;
    this->swims = true;
}

void Ghost::action() {
    Enemy::action();
    
    if(pow(this->x - player.x, 2) + pow(this->y - player.y, 2) <= 1 && this->damage_timer == 0) {
        player.health -= ceil(0.25*player.difficulty*player.level/player.resistance);
        player.damaged = fps;
        player.vel_x = 5*(player.x - this->x);
        player.vel_y = 5*(player.y - this->y);
        update_scene(false);
        this->damage_timer = fps;
    }
}

Projectile::Projectile(double x, double y, double target_x, double target_y) : Entity(1, 1, x, y) {
    this->target_x = target_x;
    this->target_y = target_y;
}

void Projectile::action() {
    this->x += (this->target_x - this->x)/fps;
    this->y += (this->target_y - this->y)/fps;
    this->elapsed_time++;
    this->sprite_state = (this->sprite_state + 1) % (10*this->sprite->states[0]);
}

BasicProjectile::BasicProjectile(double x, double y, double target_x, double target_y, int damage) : Projectile(x, y, target_x, target_y) {
    this->sprite = sprites[static_cast<int>(SpritesEnum::BasicProjectileSprite)];
    this->damage = damage;
    this->type = EntitiesEnum::BasicProjectileEntity;
};

void BasicProjectile::action() {
    Projectile::action();

    for(auto i : entities) {
        if(dynamic_cast<Enemy*>(i) && pow(this->x - i->x, 2) + pow(this->y - i->y, 2) <= 1) {
            this->health = 0;
            break;
        }
    }

    if(this->elapsed_time >= 10*fps)
        this->health = 0;

    if(this->health == 0) {
        effects.push_back(new Effect(this->x, this->y, fps, sprites[static_cast<int>(SpritesEnum::BasicProjectileExplosionSprite)]));
        explosion(this->x, this->y, damage, 1, false);
    }
}

void spawn_enemy() {
    if(rand() % spawn_rate*player.difficulty >= entities.size()) {
        int x = player.x + (rand() % 2 == 0 ? -1 : 1) * (16 + rand() % 32);
        int y = player.y + (rand() % 2 == 0 ? -1 : 1) * (16 + rand() % 32);

        if(rand() % 3 == 0)
            entities.push_back(new Ghost(x, y));
        else if(blocks[get_block(x, y)]->walkable)
            entities.push_back(new Skeleton(x, y));
        else
            entities.push_back(new Shark(x, y));
    }
}

void clear_entities() {
    while(!entities.empty()) {
        delete entities.back();
        entities.pop_back();
    }
}