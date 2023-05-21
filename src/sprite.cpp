#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "sprite.h"
#include "location.h"
#include "io.h"

using namespace std;

Sprite *sprites[static_cast<int>(SpritesEnum::END)];

Sprite::Sprite(vector<pair<std::string, int>> states_path_amount) {
    for(auto &i : states_path_amount) {
        SDL_Surface *texture_surface = IMG_Load_RW(SDL_RWFromFile(i.first.c_str(), "r"), 1);
        this->states.push_back(i.second);
        this->states_w.push_back(texture_surface->w);
        this->states_h.push_back(texture_surface->h / i.second);
        this->textures.push_back(SDL_CreateTextureFromSurface(renderer, texture_surface));
        SDL_FreeSurface(texture_surface);
    }
}

void Sprite::draw_sprite(double x, double y, int state_type, int state, bool flip, int damaged) {
    pair<int, int> coords = world_to_screen(x, y);
    
    int height = (int) (this->states_h[state_type] * ((double)block_size / this->states_w[state_type]));
    SDL_Rect src = {0, state*this->states_h[state_type], this->states_w[state_type], this->states_h[state_type]};
    SDL_Rect dest = {coords.first, coords.second - height + block_size/2, block_size, height};

    if(damaged > 0)
        SDL_SetTextureColorMod(this->textures[state_type], 255, 0, 0);
    
    SDL_RenderCopyEx(renderer, this->textures[state_type], &src, &dest, 0, nullptr, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    
    SDL_SetTextureColorMod(this->textures[state_type], 255, 255, 255);
}

Sprite::~Sprite() {
    for(auto i : this->textures)
        SDL_DestroyTexture(i);
}

void init_sprites() {
    sprites[static_cast<int>(SpritesEnum::PlayerSprite)] = new Sprite({{"resources/Blue_witch/B_witch_idle.png", 6}, {"resources/Blue_witch/B_witch_run.png", 8}});
    sprites[static_cast<int>(SpritesEnum::SkeletonSprite)] = new Sprite({{"resources/Skeleton/skeleton-idle.png", 6}, {"resources/Skeleton/skeleton-walk.png", 4}});
    sprites[static_cast<int>(SpritesEnum::BasicProjectileSprite)] = new Sprite({{"resources/BasicProjectile/basic_projectile.png", 4}});
    sprites[static_cast<int>(SpritesEnum::BasicProjectileExplosionSprite)] = new Sprite({{"resources/BasicProjectile/basic_projectile_explosion.png", 4}});
    sprites[static_cast<int>(SpritesEnum::SharkSprite)] = new Sprite({{"resources/shark.png", 8}});
    sprites[static_cast<int>(SpritesEnum::GhostSprite)] = new Sprite({{"resources/ghost.png", 4}});
    sprites[static_cast<int>(SpritesEnum::BombExplosionSprite)] = new Sprite({{"resources/explosion.png", 4}});
    sprites[static_cast<int>(SpritesEnum::TeleportationSprite)] = new Sprite({{"resources/teleportation.png", 10}});
}

void deinit_sprites() {
    for(int i = 0; i < static_cast<int>(SpritesEnum::END); i++)
        delete sprites[i];
}