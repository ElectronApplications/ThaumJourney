#ifndef RPG_IO_H
#define RPG_IO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "location.h"

extern std::string about_text;
extern std::string reference_text;

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;

void io_thread();
void poll_events();

extern int block_size;
extern int window_width;
extern int window_height;
extern double fps;
extern bool wasd[];
extern bool is_running;
extern bool quitting;

#endif