#ifndef PLAYER_H
#define PLAYER_H

#include "level.h"

struct Player {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *src_rects;
        const bool *keystate;
        struct Entity entity;
        bool ground;
        bool jumping;
        bool enable_jump;
        float scale;
        bool edit;
};

bool player_new(struct Player **player, SDL_Renderer *renderer, float scale);
void player_free(struct Player **player);
void player_edit_tog(struct Player *p);
void player_update(struct Player *p, float dt, const struct Level *level);
float player_pos(struct Player *p);
void player_draw(const struct Player *p, float camera);

#endif
