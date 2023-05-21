#ifndef RPG_GUI_H
#define RPG_GUI_H

#include <SDL2/SDL.h>
#include <string>

#include "io.h"

class GuiElement {
   public:
    SDL_Texture *texture = nullptr;
    SDL_Texture *hover_texture = nullptr;
    SDL_Texture *selected_texture = nullptr;

    int x, y, w, h;
    bool hovered = false;

    bool hidden = false;
    int token = 0;
    void (*on_hover)(GuiElement*) = nullptr;
    void (*on_click)(GuiElement*) = nullptr;

    void draw();
    virtual void update() {};
    virtual void hover_handler();
    virtual void click_handler();
    virtual void key_pressed_handler(SDL_Keycode key) {};
    GuiElement(int x, int y, int w, int h);
    virtual ~GuiElement();
};

class Solid : public GuiElement {
   public:
    SDL_Color color;
    void update();
    Solid(int x, int y, int w, int h, SDL_Color color);
};

class Label : public GuiElement {
   public:
    std::string text;
    int wrap;
    SDL_Color color;
    SDL_Color hover_color;
    SDL_Color background_color;
    SDL_Color hover_background_color;
    void update();
    Label(int x, int y, int w, int h, std::string text, int wrap, SDL_Color color, SDL_Color hover_color, SDL_Color background_color, SDL_Color hover_background_color);
};

class Button : public GuiElement {
   public:
    std::string text;
    SDL_Color background_color;
    SDL_Color hover_background_color;
    void update();
    Button(int x, int y, int w, int h, std::string text, SDL_Color background_color, SDL_Color hover_background_color);
};

class TextBox : public GuiElement {
   public:
    bool hidden_text;
    std::string text;
    void key_pressed_handler(SDL_Keycode key);
    void update();
    TextBox(int x, int y, int w, int h, std::string text, bool hidden_text);
};

class ProgressBar : public GuiElement {
   public:
    int max_amount;
    int amount;
    SDL_Color background_color;
    SDL_Color bars_color;
    void update();
    ProgressBar(int x, int y, int w, int h, int amount, int max_amount, SDL_Color background_color, SDL_Color bars_color);
};

#endif