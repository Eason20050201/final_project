#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

SDL_Texture* load_texture(const char *file, SDL_Renderer *renderer);
void render_texture_fullscreen(SDL_Texture *texture, SDL_Renderer *renderer, int window_width, int window_height);
void render_dialog_box(SDL_Renderer *renderer, const char *text, int x, int y, int w, int h);
void render_button(SDL_Renderer *renderer, const char *text, int x, int y, int w, int h);
void render_text(SDL_Renderer *renderer, const char *text, int x, int y, int w, int h);
void render_inventory_icon(SDL_Renderer *renderer, int x, int y);
void render_inventory(SDL_Renderer *renderer, int x, int y, int w, int h, const char **items, int num_items);

#endif // GRAPHICS_H
