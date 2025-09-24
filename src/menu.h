#ifndef MENU_H
#define MENU_H

#include "main.h"

struct Menu {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect rect;
        float x;
        float y;
        float w;
        float h;
        float scale;
};

bool menu_new(struct Menu **menu, SDL_Renderer *renderer, float scale);
void menu_free(struct Menu **menu);
void menu_draw(const struct Menu *m, float camera);

#endif
