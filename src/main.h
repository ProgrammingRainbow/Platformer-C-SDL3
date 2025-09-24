#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum EntityType {
    ENTITY_PLAYER,
};

struct Entity {
        float x;
        float x_vel;
        float x_off;
        float w;
        float y;
        float y_vel;
        float y_off;
        float h;
        unsigned rest;
        unsigned jump;
        unsigned walk_start;
        unsigned walk_end;
        float walk;
        unsigned image;
        SDL_FlipMode flip;
};

#define SDL_FLAGS SDL_INIT_VIDEO

#define WINDOW_TITLE "Platformer"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 32
#define PLAYER_SPEED 200

#define PLAYER_WALK_VEL 90
#define PLAYER_RUN_VEL 150
// #define PLAYER_ACCEL 3.28125
#define PLAYER_ACCEL (float)196.875
#define PLAYER_SKID (float)7.5
#define PLAYER_JUMP_VEL (float)206.25
#define PLAYER_JUMP_DEC 225
#define GRAVITY 1125
#define FALL_VEL (float)258.75

#define TARGET_FPS 40

#define PIECE_SIZE 16

#define GAME_SCALE 4

#endif
