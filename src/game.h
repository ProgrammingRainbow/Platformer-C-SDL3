#ifndef GAME_H
#define GAME_H

#include "fps.h"
#include "main.h"
#include "menu.h"
#include "player.h"

struct Game {
        SDL_Window *window;
        SDL_Renderer *renderer;
        struct Player *player;
        struct Level *level;
        struct Menu *menu;
        struct Fps *fps;
        float dt;
        float camera;
        float scale;
        SDL_Event event;
        bool is_running;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
bool game_run(struct Game *g);

#endif
