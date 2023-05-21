#ifndef RPG_SPRITE_H
#define RPG_SPRITE_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>

enum class SpritesEnum {
    PlayerSprite,
    SkeletonSprite,
    BasicProjectileSprite,
    BasicProjectileExplosionSprite,
    SharkSprite,
    GhostSprite,
    TeleportationSprite,
    BombExplosionSprite,
    END
};

class Sprite {
   public:
    std::vector<int> states;
    std::vector<int> states_w;
    std::vector<int> states_h;
    std::vector<SDL_Texture*> textures;

    void draw_sprite(double x, double y, int state_type, int state, bool flip, int damaged);
    Sprite() {};
    Sprite(std::vector<std::pair<std::string, int>> states_path_amount);
    virtual ~Sprite();
};

extern Sprite *sprites[];

void init_sprites();
void deinit_sprites();

#endif