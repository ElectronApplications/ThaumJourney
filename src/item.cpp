#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "game.h"
#include "entity.h"
#include "effect.h"
#include "item.h"
#include "scene.h"
#include "io.h"

using namespace std;

BasicFocus default_focus("Стандартная линза", "Стреляет магическими минами, которые при соприкосновении с врагами, наносят им 3 единицы урона.", 1, 0, 3);

Item::Item(string name, string description) {
    this->name = name;
    this->description = description;
}

bool UsableItem::use() {
    return true;
}

HealthPack::HealthPack(string name, string description, int uses, int regen) : UsableItem(name, description) {
    this->uses_left = uses;
    this->regen = regen;
    this->type = ItemsEnum::HealthPack;
}

bool HealthPack::use() {
    this->uses_left--;
    player.health = min(player.max_health, player.health + this->regen);
    this->description = "Восстанавливает " + to_string(this->regen) + " здоровья\nИспользований осталось: " + to_string(this->uses_left);
    
    return this->uses_left <= 0;
}

ManaPack::ManaPack(string name, string description, int uses, int regen) : UsableItem(name, description) {
    this->uses_left = uses;
    this->regen = regen;
    this->type = ItemsEnum::ManaPack;
}

bool ManaPack::use() {
    this->uses_left--;
    player.mana = min(player.max_mana, player.mana + this->regen);
    this->description = "Восстанавливает " + to_string(this->regen) + " маны\nИспользований осталось: " + to_string(this->uses_left);
    
    return this->uses_left <= 0;
}

WandFocus::WandFocus(string name, string description, int mana_usage, int intelligence_requirement) : Item(name, description) {
    this->mana_usage = mana_usage;
    this->intelligence_requirement = intelligence_requirement;
}

BasicFocus::BasicFocus(string name, string description, int mana_usage, int intelligence_requirement, int damage) : WandFocus(name, description, mana_usage, intelligence_requirement) {
    this->type = ItemsEnum::BasicFocus;
    this->damage = damage;
}

void BasicFocus::spell(double x, double y) {
    if(player.mana >= this->mana_usage && player.intelligence >= this->intelligence_requirement && pow(player.x - x, 2) + pow(player.y - y, 2) <= 32*32) {
        entities.push_back(new BasicProjectile(player.x, player.y, x, y, damage));
        player.mana -= this->mana_usage;
        update_scene(false);
    }
}

TeleportationFocus::TeleportationFocus(string name, string description, int mana_usage, int intelligence_requirement, int range) : WandFocus(name, description, mana_usage, intelligence_requirement) {
    this->type = ItemsEnum::TeleportationFocus;
    this->range = range;
}

void TeleportationFocus::spell(double x, double y) {
    if(player.mana >= this->mana_usage && player.intelligence >= this->intelligence_requirement && pow(player.x - x, 2) + pow(player.y - y, 2) <= pow(this->range, 2)) {
        effects.push_back(new Effect(player.x, player.y, 3*fps, sprites[static_cast<int>(SpritesEnum::TeleportationSprite)]));
        player.x = x;
        player.y = y;
        effects.push_back(new Effect(player.x, player.y, 3*fps, sprites[static_cast<int>(SpritesEnum::TeleportationSprite)]));
        player.mana -= this->mana_usage;
        update_scene(false);
    }
}

ExplosionFocus::ExplosionFocus(string name, string description, int mana_usage, int intelligence_requirement, int damage) : WandFocus(name, description, mana_usage, intelligence_requirement) {
    this->type = ItemsEnum::ExplosionFocus;
    this->damage = damage;
}

void ExplosionFocus::spell(double x, double y) {
    if(player.mana >= this->mana_usage && player.intelligence >= this->intelligence_requirement && pow(player.x - x, 2) + pow(player.y - y, 2) <= 32*32) {
        effects.push_back(new Effect(x, y, fps, sprites[static_cast<int>(SpritesEnum::BombExplosionSprite)]));
        explosion(x, y, damage, 3, true);
        player.mana -= this->mana_usage;
        update_scene(false);
    }
}

void Inventory::clear_inventory() {
    for(int i = 0; i < 3; i++)
        this->equipped_foci[i] = nullptr;

    while(!this->items.empty()) {
        if(this->items.back() != nullptr)
            delete this->items.back();
        this->items.pop_back();
    }
}

Inventory::~Inventory() {
    clear_inventory();
}