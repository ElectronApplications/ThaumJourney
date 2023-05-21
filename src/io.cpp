#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>

#include "io.h"
#include "location.h"
#include "gui.h"
#include "scene.h"
#include "entity.h"
#include "effect.h"
#include "loader.h"
#include "block.h"
#include "item.h"
#include "game.h"

using namespace std;

string about_text;
string reference_text;

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;

int window_width = 800;
int window_height = 600;

Uint64 prev_ticks = 0;
double fps = 60;

int block_size = 96;

bool wasd[] = {false, false, false, false};
bool is_running = true;
bool quitting = false;

void io_thread() {
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    window = SDL_CreateWindow("Thaumaturge's Journey", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(window, window_width, window_height);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    font = TTF_OpenFont("resources/font.ttf", 96);

    SDL_Surface *background_surface = IMG_Load_RW(SDL_RWFromFile("resources/background.png", "r"), 1);
    SDL_Texture *background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);

    init_blocks();
    init_sprites();
    init_structures();

    update_scene(true);

    while (is_running) {
        poll_events();

        if(current_scene != SceneEnum::GameScene && current_scene != SceneEnum::PauseScene && current_scene != SceneEnum::InventoryScene && current_scene != SceneEnum::UpgradeScene && current_scene != SceneEnum::ExitSaveScene && current_scene != SceneEnum::DeathScene && current_scene != SceneEnum::ConsoleScene) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);

            SDL_Rect src = {0, 0, background_surface->w, background_surface->h};
            if((double)window_width/window_height >= (double)background_surface->w/background_surface->h) {
                src.h = window_height / ((double)window_width / background_surface->w);
                src.y += (background_surface->h - src.h) / 2;
            }
            else {
                src.w = window_width / ((double)window_height / background_surface->h);
                src.x += (background_surface->w - src.w) / 2;
            }

            SDL_RenderCopy(renderer, background_texture, &src, nullptr);
        }
        else if(current_scene == SceneEnum::InventoryScene || current_scene == SceneEnum::UpgradeScene || current_scene == SceneEnum::ConsoleScene) {
            SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
            SDL_RenderClear(renderer);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 104, 181, 223, 255);
            SDL_RenderClear(renderer);
        }

        if(current_scene == SceneEnum::GameScene) {
            player.action();
            
            for(int i = entities.size()-1; i >= 0; i--) {
                entities[i]->action();
                if(entities[i]->health <= 0 || pow(entities[i]->x - player.x, 2) + pow(entities[i]->y - player.y, 2) > 128*128) {
                    if(entities[i]->health <= 0 && dynamic_cast<Enemy*>(entities[i]) != nullptr) {
                        if(rand() % 2 == 0)
                            obtain_loot(entities[i]->max_health);
                        player.xp += ceil(entities[i]->max_health/10.0);
                        update_scene(false);
                    }
                    entities.erase(entities.begin() + i);
                }
            }

            for(int i = effects.size()-1; i >= 0; i--) {
                effects[i]->action();
                if(effects[i]->elapsed_time >= effects[i]->alive_time) {
                    effects.erase(effects.begin() + i);
                }
            }

            load_chunks();
            spawn_enemy();
            
            if(player.health <= 0) {
                current_scene = SceneEnum::DeathScene;
                update_scene(false);
            }

            if(gui_elements[3]->token > 0) {
                gui_elements[3]->token--;
                if(gui_elements[3]->token == 0)
                    dynamic_cast<Label*>(gui_elements[3])->text = "";
            }
        }

        if(current_scene == SceneEnum::GameScene || current_scene == SceneEnum::PauseScene || current_scene == SceneEnum::ExitSaveScene || current_scene == SceneEnum::DeathScene) {
            for(int x = center_x - render_distance/2; x <= center_x + render_distance/2; x++) {
                for(int y = center_y - render_distance/2; y <= center_y + render_distance/2; y++) {
                    draw_chunk(loaded_chunks[{x, y}]);
                }
            }

            for(auto i : entities)
                i->draw();

            player.draw();

            for(auto i : effects)
                i->draw();
        }

        draw_gui();

        SDL_RenderPresent(renderer);

        Uint64 ticks = SDL_GetTicks64();
        fps = 1000.0 / (ticks - prev_ticks);
        prev_ticks = ticks;
    }

    SDL_FreeSurface(background_surface);
    SDL_DestroyTexture(background_texture);
    deinit_sprites();
    deinit_blocks();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                if(current_scene == SceneEnum::GameScene || current_scene == SceneEnum::PauseScene || current_scene == SceneEnum::InventoryScene || current_scene == SceneEnum::UpgradeScene || current_scene == SceneEnum::DeathScene || current_scene == SceneEnum::ConsoleScene) {
                    quitting = true;
                    current_scene = SceneEnum::ExitSaveScene;
                    update_scene(true);
                }
                else
                    is_running = false;
                break;
            case SDL_KEYDOWN:
                if(current_scene == SceneEnum::GameScene) {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            wasd[0] = true;
                            break;
                        case SDL_SCANCODE_A:
                            wasd[1] = true;
                            break;
                        case SDL_SCANCODE_S:
                            wasd[2] = true;
                            break;
                        case SDL_SCANCODE_D:
                            wasd[3] = true;
                            break;
                    }
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                        wasd[0] = false;
                        break;
                    case SDL_SCANCODE_A:
                        wasd[1] = false;
                        break;
                    case SDL_SCANCODE_S:
                        wasd[2] = false;
                        break;
                    case SDL_SCANCODE_D:
                        wasd[3] = false;
                        break;
                    case SDL_SCANCODE_1: case SDL_SCANCODE_2: case SDL_SCANCODE_3:
                        if(current_scene == SceneEnum::GameScene) {
                            int focus = event.key.keysym.scancode - SDL_SCANCODE_1;
                            int mouse_x, mouse_y;
                            SDL_GetMouseState(&mouse_x, &mouse_y);
                            pair coords = screen_to_world(mouse_x, mouse_y);
                            
                            if(inventory.equipped_foci[focus] == nullptr) {
                                default_focus.spell(coords.first, coords.second);
                            }
                            else {
                                inventory.equipped_foci[focus]->spell(coords.first, coords.second);
                            }
                        }
                        break;
                }
                key_pressed_gui(event.key.keysym.sym);
                break;
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                    
                    if(current_scene == SceneEnum::GameScene) {
                        change_render_distance();
                    }
                    update_scene(false);
                }
                break;
            case SDL_MOUSEWHEEL:
                if(current_scene == SceneEnum::GameScene && !(block_size == 96 && event.wheel.y > 0) && !(block_size == 16 && event.wheel.y < 0)) {
                    block_size += event.wheel.y*block_size/10;
                    if(block_size > 96)
                        block_size = 96;
                    else if(block_size < 16)
                        block_size = 16;

                    change_render_distance();
                }
                break;
            case SDL_MOUSEBUTTONUP:
                click_gui(event.button.x, event.button.y);
                break;
        }
    }

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    hover_gui(mouse_x, mouse_y);
}
