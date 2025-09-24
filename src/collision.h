#ifndef COLLISION_H
#define COLLISION_H

#include "level.h"

void check_left(struct Entity *entity, const struct Level *level);
void check_right(struct Entity *entity, const struct Level *level);
void check_top(struct Entity *entity, const struct Level *level);
bool check_bottom(struct Entity *entity, const struct Level *level);

#endif
