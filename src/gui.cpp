#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

#include "gui.h"
#include "scene.h"

using namespace std;

void GuiElement::hover_handler() {
    hovered = true;
    if(this->on_hover != nullptr)
        this->on_hover(this);
}

void GuiElement::click_handler() {
    if(this->on_click != nullptr)
        this->on_click(this);
}

void GuiElement::draw() {
    SDL_Texture *texture = this->texture;
    
    if(this == gui_elements[selected]) {
        texture = this->selected_texture;
    }
    else if(hovered) {
        texture = this->hover_texture;
        hovered = false;
    }

    SDL_Rect dest = {this->x, this->y, this->w, this->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
}

GuiElement::GuiElement(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

GuiElement::~GuiElement() {
    SDL_DestroyTexture(this->texture);
    SDL_DestroyTexture(this->hover_texture);
}

Solid::Solid(int x, int y, int w, int h, SDL_Color color) : GuiElement(x, y, w, h) {
    this->color = color;
    update();
}

void Solid::update() {
    if(this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
    }

    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->hover_texture = this->texture;
    this->selected_texture = this->texture;
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, this->texture);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, nullptr);
}

Label::Label(int x, int y, int w, int h, string text, int wrap, SDL_Color color, SDL_Color hover_color, SDL_Color background_color, SDL_Color hover_background_color) : GuiElement(x, y, w, h) {
    this->text = text;
    this->wrap = wrap;
    this->color = color;
    this->hover_color = hover_color;
    this->background_color = background_color;
    this->hover_background_color = hover_background_color;

    update();
}

void Label::update() {
    if(this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
        SDL_DestroyTexture(this->hover_texture);
    }

    SDL_Surface *text_surface = TTF_RenderUTF8_Solid_Wrapped(font, this->text.c_str(), color, wrap*96);
    SDL_Surface *hover_text_surface = TTF_RenderUTF8_Solid_Wrapped(font, text.c_str(), hover_color, wrap*96);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_Texture *hover_text_texture = SDL_CreateTextureFromSurface(renderer, hover_text_surface);

    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->hover_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->selected_texture = this->hover_texture;
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->hover_texture, SDL_BLENDMODE_BLEND);

    SDL_Rect dest = {0, 0, this->w, this->h};
    if(this->text != "") {
        if((double)this->w / this->h <= (double)text_surface->w / text_surface->h) {
            dest.h = text_surface->h / ((double)text_surface->w / this->w);
            dest.y += (this->h - dest.h) / 2;
        }
        else {
            dest.w = text_surface->w / ((double)text_surface->h / this->h);
            dest.x += (this->w - dest.w) / 2;
        }
    }

    SDL_SetRenderTarget(renderer, this->texture);
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, text_texture, nullptr, &dest);

    SDL_SetRenderTarget(renderer, this->hover_texture);
    SDL_SetRenderDrawColor(renderer, hover_background_color.r, hover_background_color.g, hover_background_color.b, hover_background_color.a);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, hover_text_texture, nullptr, &dest);

    SDL_SetRenderTarget(renderer, nullptr);
    
    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(hover_text_surface);
    SDL_DestroyTexture(text_texture);
    SDL_DestroyTexture(hover_text_texture);
}

Button::Button(int x, int y, int w, int h, string text, SDL_Color background_color, SDL_Color hover_background_color) : GuiElement(x, y, w, h) {
    this->text = text;
    this->background_color = background_color;
    this->hover_background_color = hover_background_color;

    update();
}

void Button::update() {
    if(this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
        SDL_DestroyTexture(this->hover_texture);
    }

    SDL_Surface *text_surface = TTF_RenderUTF8_Solid_Wrapped(font, this->text.c_str(), {0, 0, 0}, 0);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_Rect dest = {10, 10, this->w-20, this->h-20};

    if(this->text != "") {
        if((double)(this->w - 20) / (this->h - 20) <= (double)text_surface->w / text_surface->h) {
            dest.h = text_surface->h / ((double)text_surface->w / (this->w - 20));
            dest.y += ((h - 20) - dest.h) / 2;
        }
        else {
            dest.w = text_surface->w / ((double)text_surface->h / (this->h - 20));
            dest.x += ((w - 20) - dest.w) / 2;
        }
    }

    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->hover_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->hover_texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, this->texture);
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    SDL_RenderCopy(renderer, text_texture, nullptr, &dest);

    SDL_SetRenderTarget(renderer, this->hover_texture);
    SDL_SetRenderDrawColor(renderer, hover_background_color.r, hover_background_color.g, hover_background_color.b, hover_background_color.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    SDL_RenderCopy(renderer, text_texture, nullptr, &dest);

    SDL_SetRenderTarget(renderer, nullptr);

    this->selected_texture = this->hover_texture;

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
};

TextBox::TextBox(int x, int y, int w, int h, string text, bool hidden_text) : GuiElement(x, y, w, h) {
    this->text = text;
    this->hidden_text = hidden_text;

    this->update();
}

void TextBox::key_pressed_handler(SDL_Keycode key) {
    if(key == SDLK_BACKSPACE && this->text != "")
        this->text.pop_back();
    else if(key >= ' ' && key <= 'z')
        this->text += key;
    
    this->update();
}

void TextBox::update() {
    if(this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
        SDL_DestroyTexture(this->hover_texture);
        SDL_DestroyTexture(this->selected_texture);
    }

    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    if(this->text != "") {
        if(this->hidden_text)
            text_surface = TTF_RenderUTF8_Solid(font, string(this->text.size(), '*').c_str(), {0, 0, 0});
        else
            text_surface = TTF_RenderUTF8_Solid(font, this->text.c_str(), {0, 0, 0});
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    }

    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->hover_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->selected_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    
    SDL_Rect dest = {10, 10, w-40, h-20};

    if(this->text != "") {
        if(text_surface->w != 0 && text_surface->h != 0) {
            if((double)(this->w - 20) / (this->h - 20) <= (double)text_surface->w / text_surface->h) {
                dest.h = text_surface->h / ((double)text_surface->w / (this->w - 40));
                dest.y += ((h - 20) - dest.h) / 2;
            }
            else {
                dest.w = text_surface->w / ((double)text_surface->h / (this->h - 20));
            }
        }
    }

    SDL_SetRenderTarget(renderer, this->texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    if(this->text != "")
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest);
    
    SDL_SetRenderTarget(renderer, this->hover_texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    if(this->text != "")
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest);
    
    SDL_SetRenderTarget(renderer, this->selected_texture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if(this->text != "")
        SDL_RenderDrawLine(renderer, (dest.x + dest.w)/8 + 1, 2, (dest.x + dest.w)/8 + 1, h/8 - 2);
    else
        SDL_RenderDrawLine(renderer, 2, 2, 2, h/8 - 2);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    if(this->text != "")
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest);

    SDL_SetRenderTarget(renderer, nullptr);
    
    if(this->text != "") {
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
    }
}

ProgressBar::ProgressBar(int x, int y, int w, int h, int amount, int max_amount, SDL_Color background_color, SDL_Color bars_color) : GuiElement(x, y, w, h) {
    this->amount = amount;
    this->max_amount = max_amount;
    this->background_color = background_color;
    this->bars_color = bars_color;

    this->update();
}

void ProgressBar::update() {
    if(this->texture != nullptr)
        SDL_DestroyTexture(this->texture);

    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    this->hover_texture = this->texture;
    this->selected_texture = this->texture;
    SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, this->texture);
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderSetScale(renderer, 8.0, 8.0);
    SDL_RenderDrawRect(renderer, nullptr);
    SDL_RenderSetScale(renderer, 1.0, 1.0);

    SDL_SetRenderDrawColor(renderer, bars_color.r, bars_color.g, bars_color.b, bars_color.a);
    int bar_width = (double)(this->w - 20 - 2 * (max_amount - 1)) / max_amount;
    for(int i = 0; i < this->amount; i++) {
        SDL_Rect dest = {10 + 2*i + i * bar_width, 10, bar_width, this->h - 20};
        SDL_RenderFillRect(renderer, &dest);
    }

    SDL_SetRenderTarget(renderer, nullptr);
}
