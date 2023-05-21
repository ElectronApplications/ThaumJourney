#ifndef RPG_SCENE_H
#define RPG_SCENE_H

#include <vector>
#include <SDL2/SDL.h>

#include "gui.h"

enum class SceneEnum {
    MainScene,
    NewGameScene,
    LoadGameScene,
    LoginScene,
    AboutScene,
    ReferenceScene,
    GameScene,
    PauseScene,
    InventoryScene,
    UpgradeScene,
    ExitSaveScene,
    DeathScene,
    ConsoleScene,
};

extern SceneEnum current_scene;
extern std::vector<GuiElement*> gui_elements;
extern int selected;

void update_scene(bool new_scene);
void main_scene(bool new_scene);
void new_game_scene(bool new_scene);
void load_game_scene(bool new_scene);
void about_scene(bool new_scene);
void reference_scene(bool new_scene);
void game_scene(bool new_scene);
void pause_scene(bool new_scene);
void login_scene(bool new_scene);
void inventory_scene(bool new_scene);
void upgrade_scene(bool new_scene);
void exit_save_scene(bool new_scene);
void death_scene(bool new_scene);
void console_scene(bool new_scene);

void clear_gui();
void draw_gui();
void hover_gui(int x, int y);
void click_gui(int x, int y);
void key_pressed_gui(SDL_Keycode key);

#endif