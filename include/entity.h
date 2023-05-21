#ifndef RPG_ENTITY_H
#define RPG_ENTITY_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "item.h"
#include "sprite.h"

enum class EntitiesEnum {
    BaseEntity,
    SkeletonEntity,
    SharkEntity,
    GhostEntity,
    BasicProjectileEntity,
};

class Entity {
   public:
    EntitiesEnum type = EntitiesEnum::BaseEntity;

    int health = 10;
    int max_health = 10;
    double x = 0;
    double y = 0;

    int damaged = 0;
    bool flip = false;
    int sprite_state_type = 0;
    int sprite_state = 0;
    Sprite *sprite;

    virtual void action() {};
    void draw();
    Entity() {};
    Entity(int health, int max_health, double x, double y);
};

class Player : public Entity {
   public:
    std::string player_name;
    size_t password = 0;
    time_t save_time = 0;

    int health_regen = 1;
    int mana = 5;
    int max_mana = 5;
    int mana_regen = 1;
    int resistance = 1;
    int intelligence = 0;
    int dexterity = 2;
    int xp = 0;
    int level = 1;
    int used_points = 0;
    int difficulty = 1;
    int seed = 0;

    int regen_timer = 0;
    double vel_x = 0;
    double vel_y = 0;

    void action();
    Player() { this->sprite = sprites[static_cast<int>(SpritesEnum::PlayerSprite)]; };
    Player(Entity &entity, std::string player_name, size_t password, time_t save_time, int health_regen, int mana, int max_mana, int mana_regen, int resistance, int intelligence, int dexterity, int xp, int level, int used_points, int difficulty, int seed);
};

class Enemy : public Entity {
   public:
    bool walks = true;
    bool swims = false;
    
    int damage_timer = 0;
    double vel_x = 0;
    double vel_y = 0;

    void action();
    Enemy(int max_health, double x, double y) : Entity(max_health, max_health, x, y) {};
};

class Skeleton : public Enemy {
   public:
    void action();
    Skeleton(double x, double y);
};

class Shark : public Enemy {
   public:
    void action();
    Shark(double x, double y);
};

class Ghost : public Enemy {
   public:
    void action();
    Ghost(double x, double y);
};

class Projectile : public Entity {
   public:
    double target_x;
    double target_y;

    int elapsed_time = 0;

    void action();
    Projectile(double x, double y, double target_x, double target_y);
};

class BasicProjectile : public Projectile {
   public:
    int damage;
    void action();
    BasicProjectile(double x, double y, double target_x, double target_y, int damage);
};

extern int spawn_rate;
extern std::vector<Entity*> entities;

void spawn_enemy();
void clear_entities();

#endif