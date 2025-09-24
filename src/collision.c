#include "collision.h"

void check_left(struct Entity *entity, const struct Level *level) {
    unsigned top = (unsigned)(entity->y + entity->y_off) / PIECE_SIZE;
    unsigned center = (unsigned)(entity->y + PIECE_SIZE) / PIECE_SIZE;
    unsigned bottom = (unsigned)(entity->y + entity->h - 1) / PIECE_SIZE;
    unsigned left = (unsigned)(entity->x + entity->x_off) / PIECE_SIZE;

    if (entity->x + entity->x_off < 0) {
        entity->x = -entity->x_off;
        entity->x_vel = 0;
        return;
    }
    if (top < level->rows && bottom < level->rows) {
        if (level->tiles[top * level->columns + left].solid) {
            entity->x = (float)(left + 1) * PIECE_SIZE - entity->x_off;
            entity->x_vel = 0;
            return;
        }
        if (entity->h > PIECE_SIZE) {
            if (level->tiles[center * level->columns + left].solid) {
                entity->x = (float)(left + 1) * PIECE_SIZE - entity->x_off;
                entity->x_vel = 0;
                return;
            }
        }
        if (level->tiles[bottom * level->columns + left].solid) {
            entity->x = (float)(left + 1) * PIECE_SIZE - entity->x_off;
            entity->x_vel = 0;
            return;
        }
    }
}

void check_right(struct Entity *entity, const struct Level *level) {
    unsigned top = (unsigned)(entity->y + entity->y_off) / PIECE_SIZE;
    unsigned center = (unsigned)(entity->y + PIECE_SIZE) / PIECE_SIZE;
    unsigned bottom = (unsigned)(entity->y + entity->h - 1) / PIECE_SIZE;
    unsigned right = (unsigned)ceil(entity->x + entity->w) / PIECE_SIZE;

    if (right >= level->columns) {
        entity->x = (float)right * PIECE_SIZE - entity->w;
        entity->x_vel = 0;
        return;
    }

    if (top < level->rows && bottom < level->rows) {
        if (level->tiles[top * level->columns + right].solid) {
            entity->x = (float)right * PIECE_SIZE - entity->w;
            entity->x_vel = 0;
            return;
        }
        if (entity->h > PIECE_SIZE) {
            if (level->tiles[center * level->columns + right].solid) {
                entity->x = (float)right * PIECE_SIZE - entity->w;
                entity->x_vel = 0;
                return;
            }
        }
        if (level->tiles[bottom * level->columns + right].solid) {
            entity->x = (float)right * PIECE_SIZE - entity->w;
            entity->x_vel = 0;
            return;
        }
    }
}

void check_top(struct Entity *entity, const struct Level *level) {
    unsigned top = (unsigned)(entity->y + entity->y_off) / PIECE_SIZE;
    unsigned left = (unsigned)(entity->x + entity->x_off) / PIECE_SIZE;
    unsigned middle = (unsigned)(entity->x + PIECE_SIZE) / PIECE_SIZE;
    unsigned right = (unsigned)(entity->x + entity->w - 1) / PIECE_SIZE;

    // if (entity->y + entity->y_off < 0) {
    //     entity->y = -entity->y_off;
    //     entity->y_vel = 0;
    //     entity->long_jump = false;
    //     return;
    // }
    if (top < level->columns && right < level->columns) {
        if (level->tiles[top * level->columns + left].solid) {
            entity->y = (float)(top + 1) * PIECE_SIZE - entity->y_off;
            entity->y_vel = 0;
            entity->jump = false;
            return;
        }
        if (entity->w > PIECE_SIZE) {
            if (level->tiles[top * level->columns + middle].solid) {
                entity->y = (float)(top + 1) * PIECE_SIZE - entity->y_off;
                entity->y_vel = 0;
                entity->jump = false;
                return;
            }
        }
        if (level->tiles[top * level->columns + right].solid) {
            entity->y = (float)(top + 1) * PIECE_SIZE - entity->y_off;
            entity->y_vel = 0;
            entity->jump = false;
            return;
        }
    }
}

bool check_bottom(struct Entity *entity, const struct Level *level) {
    unsigned bottom = (unsigned)ceil(entity->y + entity->h) / PIECE_SIZE;
    unsigned left = (unsigned)(entity->x + entity->x_off) / PIECE_SIZE;
    unsigned middle = (unsigned)(entity->x + PIECE_SIZE) / PIECE_SIZE;
    unsigned right = (unsigned)(entity->x + entity->w - 1) / PIECE_SIZE;

    if (bottom == level->rows) {
        entity->y = (float)bottom * PIECE_SIZE - entity->h;
        entity->y_vel = 0;
        return true;
    }

    if (bottom < level->rows && right < level->columns) {
        if (level->tiles[bottom * level->columns + left].solid) {
            entity->y = (float)bottom * PIECE_SIZE - entity->h;
            entity->y_vel = 0;
            return true;
        }
        if (entity->w > PIECE_SIZE) {
            if (level->tiles[bottom * level->columns + middle].solid) {
                entity->y = (float)bottom * PIECE_SIZE - entity->h;
                entity->y_vel = 0;
                return true;
            }
        }
        if (level->tiles[bottom * level->columns + right].solid) {
            entity->y = (float)bottom * PIECE_SIZE - entity->h;
            entity->y_vel = 0;
            return true;
        }
    }

    return false;
}
