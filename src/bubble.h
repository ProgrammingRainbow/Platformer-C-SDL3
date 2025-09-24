#ifndef BUBBLE_H
#define BUBBLE_H

#include "main.h"

SDL_Surface *bubble_surface(SDL_Surface *src_surf, int radius,
                            SDL_Color outer_color);
SDL_Surface *bubble_create_text(const char *str, float size, int radius,
                                SDL_Color inner_color, SDL_Color outer_color);
SDL_Texture *bubble_rect(SDL_Renderer *renderer, int w, int h, int radius,
                         SDL_Color color, SDL_Color bcolor, int border,
                         int shadow);

#endif
