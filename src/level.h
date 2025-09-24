#ifndef LEVEL_H
#define LEVEL_H

#include "main.h"

enum TileType {
    TILE_SKY,
    TILE_STONE,
    TILE_BRICK,
    TILE_COIN,
    TILE_COIN_BOX,
    TILE_POWER_UP,
    TILE_INVINCIBLE,
    TILE_FLOWER,
    TILE_TURTLE,
    TILE_MUSHROOM,
};

struct Tile {
        enum TileType type;
        unsigned image;
        bool solid_top;
        bool solid;
        unsigned count;
        void (*activate)(void *self);
};

struct Level {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_FRect *rects;
        struct Tile *tiles;
        float piece_size;
        unsigned rows;
        unsigned columns;
        SDL_FRect edit_rect;
        unsigned edit_index;
        unsigned edit_image;
        bool edit;
        const bool *keystate;
};

bool level_new(struct Level **level, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, float scale);
void level_free(struct Level **level);
// void level_edit(struct Level *l, float x, float y);
void level_edit_inc(struct Level *l);
void level_edit_dec(struct Level *l);
void level_edit_tog(struct Level *l);
float level_width(struct Level *l);
// bool piece_is_solid(struct Piece *p);
void level_update(struct Level *l, float camera);
bool level_save(struct Level *l);
bool level_grow(struct Level *l);
bool level_shrink(struct Level *l);
void level_draw(const struct Level *l, float camera);

#endif
