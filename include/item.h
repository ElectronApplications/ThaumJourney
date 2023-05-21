#ifndef RPG_ITEM_H
#define RPG_ITEM_H

#include <string>
#include <vector>

#include "entity.h"

enum class ItemsEnum {
    BaseItem,
    DefaultFocus,
    HealthPack,
    ManaPack,
    BasicFocus,
    TeleportationFocus,
    ExplosionFocus,
};

class Item {
   public:
    ItemsEnum type = ItemsEnum::BaseItem;
    std::string name;
    std::string description;
    virtual void _poly() {};
    Item() {};
    Item(std::string name, std::string description);
};

class UsableItem : public Item {
   public:
    void _poly() {};
    virtual bool use();
    UsableItem(std::string name, std::string description) : Item(name, description) {};
};

class HealthPack : public UsableItem {
   public:
    int uses_left;
    int regen;
    bool use();
    HealthPack(std::string name, std::string description, int uses, int regen);
};

class ManaPack : public UsableItem {
   public:
    int uses_left;
    int regen;
    bool use();
    ManaPack(std::string name, std::string description, int uses, int regen);
};

class WandFocus : public Item {
   public:
    int mana_usage;
    int intelligence_requirement;
    void _poly() {};
    virtual void spell(double x, double y) {};
    WandFocus(std::string name, std::string description, int mana_usage, int intelligence_requirement);
};

class BasicFocus : public WandFocus {
   public:
    int damage;
    void spell(double x, double y);
    BasicFocus(std::string name, std::string description, int mana_usage, int intelligence_requirement, int damage);
};

class TeleportationFocus : public WandFocus {
   public:
    int range;
    void spell(double x, double y);
    TeleportationFocus(std::string name, std::string description, int mana_usage, int intelligence_requirement, int range);
};

class ExplosionFocus : public WandFocus {
   public:
    int damage;
    void spell(double x, double y);
    ExplosionFocus(std::string name, std::string description, int mana_usage, int intelligence_requirement, int damage);
};

class Inventory {
   public:
    std::vector<Item*> items;
    WandFocus *equipped_foci[3] = {nullptr, nullptr, nullptr};
    void clear_inventory();
    virtual ~Inventory();
};

extern BasicFocus default_focus;

#endif