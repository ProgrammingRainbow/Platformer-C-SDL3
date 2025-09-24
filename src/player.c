#include "player.h"
#include "collision.h"
#include "load_media.h"

bool player_new(struct Player **player, SDL_Renderer *renderer, float scale) {
    *player = calloc(1, sizeof(struct Player));
    if (!*player) {
        fprintf(stderr, "Error in calloc of new border.\n");
        return false;
    }
    struct Player *p = *player;

    p->renderer = renderer;
    p->scale = scale;

    if (!load_media_sheet(p->renderer, &p->image, "images/Mario.png",
                          PLAYER_WIDTH, PLAYER_HEIGHT, &p->src_rects)) {
        return false;
    }

    // SDL_SetTextureColorMod(p->image, 0, 0, 0);
    // SDL_SetTextureAlphaMod(p->image, 128);

    p->entity.x = (float)(WINDOW_WIDTH - PLAYER_WIDTH) / (2 * scale);
    p->entity.x_vel = 0;
    p->entity.x_off = 4;
    p->entity.w = 16;
    p->entity.y = (float)(WINDOW_HEIGHT - PLAYER_HEIGHT) / (2 * scale);
    p->entity.y_off = 0;
    p->entity.y_vel = 0;
    p->entity.h = PLAYER_HEIGHT;
    p->entity.rest = 20;
    p->entity.jump = 22;
    p->entity.walk_start = 25;
    p->entity.walk_end = 27;
    p->entity.image = p->entity.rest;
    p->entity.flip = SDL_FLIP_HORIZONTAL;
    p->keystate = SDL_GetKeyboardState(NULL);

    return true;
}

void player_free(struct Player **player) {
    if (*player) {
        struct Player *p = *player;

        if (p->src_rects) {
            free(p->src_rects);
            p->src_rects = NULL;
        }
        if (p->image) {
            SDL_DestroyTexture(p->image);
            p->image = NULL;
        }

        p->renderer = NULL;

        free(p);
        p = NULL;
        *player = NULL;

        printf("Free Player.\n");
    }
}

void player_edit_tog(struct Player *p) { p->edit = !p->edit; }

void player_update(struct Player *p, float dt, const struct Level *level) {
    if (!p->edit) {
        bool move_h = false;
        if (p->keystate[SDL_SCANCODE_LEFT]) {
            move_h = true;
            if (p->entity.x_vel > 0) {
                p->entity.x_vel -= PLAYER_ACCEL * dt * 2;
            } else {
                p->entity.x_vel -= PLAYER_ACCEL * dt;
                if (p->entity.x_vel < -PLAYER_WALK_VEL) {
                    p->entity.x_vel = -PLAYER_WALK_VEL;
                }
            }
            p->entity.flip = SDL_FLIP_HORIZONTAL;
        }
        if (p->keystate[SDL_SCANCODE_RIGHT]) {
            move_h = true;
            if (p->entity.x_vel < 0) {
                p->entity.x_vel += PLAYER_ACCEL * dt * 2;
            } else {
                p->entity.x_vel += PLAYER_WALK_VEL * dt;
                if (p->entity.x_vel > PLAYER_WALK_VEL) {
                    p->entity.x_vel = PLAYER_WALK_VEL;
                }
            }
            p->entity.flip = SDL_FLIP_NONE;
        }

        if (p->keystate[SDL_SCANCODE_UP]) {
            if (p->enable_jump) {
                p->entity.y_vel = -PLAYER_JUMP_VEL;
                p->jumping = true;
                p->ground = false;
                p->enable_jump = false;
            }
        } else {
            if (p->ground) {
                p->enable_jump = true;
            }
            p->jumping = false;
        }

        if (p->jumping) {
            p->entity.y_vel += PLAYER_JUMP_DEC * dt;
        } else {
            p->entity.y_vel += GRAVITY * dt;
        }

        // if (p->keystate[SDL_SCANCODE_DOWN]) {
        //     p->pos.y += PLAYER_WALK_VEL * dt;
        // }

        if (!move_h) {
            if (p->entity.x_vel > PLAYER_ACCEL * dt) {
                p->entity.x_vel -= PLAYER_ACCEL * dt;
            } else if (p->entity.x_vel < -PLAYER_ACCEL * dt) {
                p->entity.x_vel += PLAYER_ACCEL * dt;
            } else {
                p->entity.x_vel = 0;
            }
        }

        p->entity.x += p->entity.x_vel * dt;

        if (p->entity.x_vel > 0) {
            check_right(&p->entity, level);
        } else if (p->entity.x_vel < 0) {
            check_left(&p->entity, level);
        }

        if (p->entity.y_vel > FALL_VEL) {
            p->entity.y_vel = FALL_VEL;
        }
        p->entity.y += p->entity.y_vel * dt;

        if (p->entity.y_vel < 0) {
            check_top(&p->entity, level);
        } else if (p->entity.y_vel > 0) {
            if (check_bottom(&p->entity, level)) {
                p->ground = true;
            } else {
                p->ground = false;
                p->enable_jump = false;
            }
        }

        if (p->ground) {
            if (p->entity.x_vel > 0 || p->entity.x_vel < 0) {
                p->entity.walk += (float)fabs(p->entity.x_vel) / 120;
                if ((unsigned)p->entity.walk < p->entity.walk_start ||
                    (unsigned)p->entity.walk > p->entity.walk_end) {
                    p->entity.walk = (float)p->entity.walk_start;
                }
                p->entity.image = (unsigned)p->entity.walk;
            } else {
                p->entity.image = p->entity.rest;
                p->entity.walk = (float)p->entity.rest;
            }
        } else if (p->jumping) {
            p->entity.image = p->entity.jump;
            p->entity.walk = (float)p->entity.jump;
        }
    }
}

float player_pos(struct Player *p) {
    return (p->entity.x + (p->entity.w / 2)) * p->scale;
}

void player_draw(const struct Player *p, float camera) {
    SDL_FRect rect = {p->entity.x * p->scale + camera, p->entity.y * p->scale,
                      PLAYER_WIDTH * p->scale, PLAYER_HEIGHT * p->scale};
    SDL_RenderTextureRotated(p->renderer, p->image,
                             &p->src_rects[p->entity.image], &rect, 0, NULL,
                             p->entity.flip);
}
