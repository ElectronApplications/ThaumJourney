#include <vector>
#include <cmath>

#include "sprite.h"
#include "effect.h"

using namespace std;

vector<Effect*> effects;

Effect::Effect(double x, double y, int alive_time, Sprite *sprite) {
    this->x = x;
    this->y = y;
    this->alive_time = alive_time;
    this->sprite = sprite;
}

void Effect::action() {
    this->elapsed_time++;
}

void Effect::draw() {
    this->sprite->draw_sprite(this->x, this->y, 0, round((double)this->sprite->states[0] * this->elapsed_time / this->alive_time), false, 0);
}

void clear_effects() {
    while(!effects.empty()) {
        delete effects.back();
        effects.pop_back();
    }
}
