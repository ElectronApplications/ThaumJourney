#ifndef RPG_EFFECT_H
#define RPG_EFFECT_H

#include <vector>

#include "sprite.h"

class Effect {
   public:
    double x, y;
    int alive_time;
    Sprite *sprite;

    int elapsed_time = 0;

    void action();
    void draw();
    Effect(double x, double y, int alive_time, Sprite *sprite);
};

extern std::vector<Effect*> effects;

void clear_effects();

#endif