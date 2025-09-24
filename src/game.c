#include "game.h"
#include "init_sdl.h"
#include "level.h"
#include "main.h"

bool game_events(struct Game *g);
void game_update(struct Game *g);
void game_camera(struct Game *g);
void game_draw(struct Game *g);

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in Calloc of New Game.\n");
        return false;
    }
    struct Game *g = *game;

    g->is_running = true;
    g->scale = GAME_SCALE;

    if (!game_init_sdl(g)) {
        return false;
    }

    if (!player_new(&g->player, g->renderer, GAME_SCALE)) {
        return false;
    }
    if (!level_new(&g->level, g->renderer, 11, 20, GAME_SCALE)) {
        return false;
    }
    // if (!menu_new(&g->menu, g->renderer, GAME_SCALE)) {
    //     return false;
    // }
    if (!fps_new(&g->fps)) {
        return false;
    }

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;

        if (g->fps) {
            fps_free(&g->fps);
        }
        // if (g->menu) {
        //     menu_free(&g->menu);
        // }
        if (g->level) {
            level_free(&g->level);
        }
        if (g->player) {
            player_free(&g->player);
        }

        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }
        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        SDL_Quit();

        free(g);
        g = NULL;
        *game = NULL;

        printf("All clean!\n");
    }
}

bool game_events(struct Game *g) {
    while (SDL_PollEvent(&g->event)) {
        switch (g->event.type) {
        case SDL_EVENT_QUIT:
            g->is_running = false;
            break;
        // case SDL_EVENT_MOUSE_BUTTON_DOWN:
        //     level_edit(g->level, g->event.button.x, g->event.button.y);
        //     break;
        case SDL_EVENT_KEY_DOWN:
            switch (g->event.key.scancode) {
            case SDL_SCANCODE_ESCAPE:
                g->is_running = false;
                break;
            case SDL_SCANCODE_E:
                level_edit_tog(g->level);
                player_edit_tog(g->player);
                break;
            case SDL_SCANCODE_UP:
                level_edit_inc(g->level);
                break;
            case SDL_SCANCODE_DOWN:
                level_edit_dec(g->level);
                break;
            case SDL_SCANCODE_RIGHT:
                if (!level_grow(g->level)) {
                    return false;
                }
                break;
            case SDL_SCANCODE_LEFT:
                if (!level_shrink(g->level)) {
                    return false;
                }
                break;

            case SDL_SCANCODE_S:
                if (!level_save(g->level)) {
                    return false;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return true;
}

void game_camera(struct Game *g) {
    float player_x = player_pos(g->player);
    float half_window = (WINDOW_WIDTH / 2.0);
    float max_level = level_width(g->level);

    if (player_x > half_window) {
        if (player_x < max_level - half_window) {
            g->camera = half_window - player_x;
        } else {
            g->camera = half_window * 2 - max_level;
        }
    } else {
        g->camera = 0;
    }
}

void game_update(struct Game *g) {
    player_update(g->player, g->dt, g->level);
    game_camera(g);
    level_update(g->level, g->camera);
}

void game_draw(struct Game *g) {
    SDL_RenderClear(g->renderer);

    level_draw(g->level, g->camera);
    player_draw(g->player, g->camera);
    // menu_draw(g->menu, g->camera);

    SDL_RenderPresent(g->renderer);
}

bool game_run(struct Game *g) {
    while (g->is_running) {
        if (!game_events(g)) {
            return false;
        }

        game_update(g);

        game_draw(g);

        g->dt = fps_update(g->fps);
    }

    return true;
}
