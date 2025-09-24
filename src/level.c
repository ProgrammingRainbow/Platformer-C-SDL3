#include "level.h"
#include "load_media.h"

bool level_load(struct Level *l);

bool level_new(struct Level **level, SDL_Renderer *renderer, unsigned rows,
               unsigned columns, float scale) {
    *level = calloc(1, sizeof(struct Level));
    if (*level == NULL) {
        fprintf(stderr, "Error in Calloc of New Level.\n");
        return false;
    }
    struct Level *l = *level;

    l->renderer = renderer;
    l->rows = rows;
    l->columns = columns;
    l->piece_size = scale * PIECE_SIZE;
    l->edit_rect = (SDL_FRect){0, 0, l->piece_size, l->piece_size};
    l->edit_index = 0;
    l->edit_image = 0;
    l->edit = false;

    if (!load_media_sheet(renderer, &l->image, "images/images.png", PIECE_SIZE,
                          PIECE_SIZE, &l->rects)) {
        return false;
    }

    if (!level_load(l)) {
        return false;
    }

    l->keystate = SDL_GetKeyboardState(NULL);

    return true;
}

void level_free(struct Level **level) {
    if (*level) {
        struct Level *l = *level;

        if (l->rects) {
            free(l->rects);
            l->rects = NULL;
        }
        if (l->image) {
            SDL_DestroyTexture(l->image);
            l->image = NULL;
        }

        if (l->tiles) {
            free(l->tiles);
            l->tiles = NULL;
        }

        free(l);
        l = NULL;
        *level = NULL;

        printf("Free Level.\n");
    }
}

void level_update(struct Level *l, float camera) {
    if (l->edit) {
        float x, y;
        SDL_MouseButtonFlags button = SDL_GetMouseState(&x, &y);

        unsigned column = (unsigned)((x - camera) / l->piece_size);
        unsigned row = (unsigned)(y / l->piece_size);

        if (row >= l->rows && column >= l->columns) {
            return;
        }

        l->edit_rect.x = (float)column * l->piece_size + camera;
        l->edit_rect.y = (float)row * l->piece_size;

        if (l->keystate[SDL_SCANCODE_LSHIFT]) {
            l->edit_image = 109;
        } else {
            l->edit_image = l->edit_index;
        }

        if (button & SDL_BUTTON_LMASK) {
            if (l->keystate[SDL_SCANCODE_LSHIFT]) {
                l->tiles[row * l->columns + column].image = 108;
                l->tiles[row * l->columns + column].solid = false;
            } else {
                if (l->tiles[row * l->columns + column].image == 108) {
                    l->tiles[row * l->columns + column].image = l->edit_index;
                    if (l->edit_index > 89) {
                        l->tiles[row * l->columns + column].solid = false;
                    } else {
                        l->tiles[row * l->columns + column].solid = true;
                    }
                }
            }
        }
    }
}

float level_width(struct Level *l) { return (float)l->columns * l->piece_size; }

void level_edit_inc(struct Level *l) {
    if (l->edit) {
        if (l->edit_index >= 109) {
            l->edit_index = 0;
        } else {
            l->edit_index++;
        }
    }
}

void level_edit_dec(struct Level *l) {
    if (l->edit) {
        if (l->edit_index <= 0) {
            l->edit_index = 109;
        } else {
            l->edit_index--;
        }
    }
}

void level_edit_tog(struct Level *l) { l->edit = !l->edit; }

bool level_save(struct Level *l) {
    unsigned count = l->rows * l->columns;

    FILE *file = fopen("testfile", "wb");
    if (file != NULL) {
        fwrite(&count, sizeof(unsigned), 1, file);
        fwrite(l->tiles, sizeof(struct Tile), count, file);
        fclose(file);
    } else {
        fprintf(stderr, "Error opening %s for writing\n", "testfile");
        return false;
    }

    return true;
}

bool level_load(struct Level *l) {

    FILE *file = fopen("testfile", "rb");
    if (file != NULL) {
        unsigned count = 0;
        fread(&count, sizeof(unsigned), 1, file);

        l->columns = count / l->rows;
        l->tiles = calloc(1, sizeof(struct Tile) * l->rows * l->columns);
        if (l->tiles == NULL) {
            fprintf(stderr, "Error in Calloc of New Pieces.\n");
            return false;
        }

        fread(l->tiles, sizeof(struct Tile), count, file);
        fclose(file);
    } else {
        l->tiles = calloc(1, sizeof(struct Tile) * l->rows * l->columns);
        if (l->tiles == NULL) {
            fprintf(stderr, "Error in Calloc of New Pieces.\n");
            return false;
        }

        for (unsigned row = 0; row < l->rows; row++) {
            for (unsigned column = 0; column < l->columns; column++) {
                l->tiles[row * l->columns + column].image = 108;
            }
        }

        for (unsigned column = 0; column < l->columns; column++) {
            l->tiles[10 * l->columns + column].image = 0;
            l->tiles[10 * l->columns + column].solid = true;
        }
    }

    return true;
}

bool level_grow(struct Level *l) {
    if (l->edit) {
        unsigned new_columns = l->columns + 1;
        struct Tile *new_tiles =
            calloc(1, sizeof(struct Tile) * l->rows * new_columns);
        if (l->tiles == NULL) {
            fprintf(stderr, "Error in Calloc of New Pieces.\n");
            return false;
        }

        for (unsigned row = 0; row < l->rows; row++) {
            for (unsigned column = 0; column < l->columns; column++) {
                unsigned index = row * l->columns + column;
                unsigned new_index = row * new_columns + column;
                new_tiles[new_index].image = l->tiles[index].image;
                new_tiles[new_index].solid = l->tiles[index].solid;
            }
        }

        free(l->tiles);
        l->columns = new_columns;
        l->tiles = new_tiles;

        for (unsigned row = 0; row < l->rows; row++) {
            l->tiles[row * l->columns + (l->columns - 1)].image = 108;
        }
    }

    return true;
}

bool level_shrink(struct Level *l) {
    if (l->edit) {
        unsigned new_columns = l->columns - 1;

        struct Tile *new_tiles =
            calloc(1, sizeof(struct Tile) * l->rows * new_columns);
        if (l->tiles == NULL) {
            fprintf(stderr, "Error in Calloc of New Pieces.\n");
            return false;
        }

        for (unsigned row = 0; row < l->rows; row++) {
            for (unsigned column = 0; column < new_columns; column++) {
                unsigned index = row * l->columns + column;
                unsigned new_index = row * new_columns + column;
                new_tiles[new_index].image = l->tiles[index].image;
                new_tiles[new_index].solid = l->tiles[index].solid;
            }
        }

        free(l->tiles);
        l->columns = new_columns;
        l->tiles = new_tiles;
    }

    return true;
}

// void level_edit(struct Level *l, float x, float y) {
//     unsigned row = (unsigned)(y / l->piece_size);
//     unsigned column = (unsigned)(x / l->piece_size);
//
//     if (row < l->rows && column < l->columns) {
//         if (l->pieces[row * l->columns + column].visible) {
//             l->pieces[row * l->columns + column].visible = false;
//             l->pieces[row * l->columns + column].solid = false;
//         } else {
//             l->pieces[row * l->columns + column].visible = true;
//             l->pieces[row * l->columns + column].solid = true;
//         }
//     }
// }

// bool piece_is_solid(struct Piece *p) {}

void level_draw(const struct Level *l, float camera) {
    SDL_FRect dest_rect = {0, 0, l->piece_size, l->piece_size};
    for (unsigned row = 0; row < l->rows; row++) {
        dest_rect.y = (float)row * l->piece_size;
        for (unsigned column = 0; column < l->columns; column++) {
            // if (l->pieces[row * l->columns + column].visible) {
            dest_rect.x = (float)column * l->piece_size + camera;
            unsigned image = l->tiles[row * l->columns + column].image;
            SDL_RenderTexture(l->renderer, l->image, &l->rects[image],
                              &dest_rect);
            // }
        }
    }

    if (l->edit) {
        SDL_RenderTexture(l->renderer, l->image, &l->rects[l->edit_image],
                          &l->edit_rect);
    }
}
