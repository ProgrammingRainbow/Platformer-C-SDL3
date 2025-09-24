#include "fps.h"

Uint64 fps_time_since(Uint64 last_time, Uint64 *new_last_time);
void fps_update_display(struct Fps *f);

bool fps_new(struct Fps **fps) {
    *fps = calloc(1, sizeof(struct Fps));
    if (!*fps) {
        fprintf(stderr, "Error in calloc of fps!\n");
        return false;
    }
    struct Fps *f = *fps;

    f->last_time = SDL_GetTicks();
    f->target_duration = 1000.0 / TARGET_FPS;
    f->carry_delay = 0;
    f->max_delay = 50;
    f->fps_last_time = f->last_time;
    f->fps_counter = 0;
    f->fps_display = false;

    return true;
}

void fps_free(struct Fps **fps) {
    if (*fps) {
        free(*fps);
        *fps = NULL;
    }
}

void fps_toggle_display(struct Fps *f) {
    if (f->fps_display) {
        f->fps_display = false;
    } else {
        f->fps_display = true;
        f->fps_counter = 0;
        f->fps_last_time = SDL_GetTicks();
    }
}

Uint64 fps_time_since(Uint64 last_time, Uint64 *new_last_time) {
    Uint64 current_time = SDL_GetTicks();
    Uint64 elapsed_time = 0;

    if (current_time < last_time) {
        elapsed_time = (Uint64)-1 - last_time + current_time;
    } else {
        elapsed_time = current_time - last_time;
    }

    if (new_last_time) {
        *new_last_time = current_time;
    }

    return elapsed_time;
}

void fps_update_display(struct Fps *f) {
    if (f->fps_display) {
        if (fps_time_since(f->fps_last_time, NULL) >= 1000) {
            printf("%lu\n", f->fps_counter);
            f->fps_counter = 0;
            f->fps_last_time += 1000;
        }
        f->fps_counter++;
    }
}

float fps_update(struct Fps *f) {
    Uint64 elapsed_time = fps_time_since(f->last_time, NULL);
    double duration = f->target_duration + f->carry_delay;

    if ((Uint64)duration > elapsed_time) {
        Uint64 delay = (Uint64)duration - elapsed_time;
        SDL_Delay((Uint32)delay);
    }

    elapsed_time = fps_time_since(f->last_time, &f->last_time);

    f->carry_delay = duration - (double)elapsed_time;
    if (f->carry_delay > f->max_delay) {
        f->carry_delay = f->max_delay;
    } else if (f->carry_delay < -f->max_delay) {
        f->carry_delay = -f->max_delay;
    }

    fps_update_display(f);

    return (float)elapsed_time / 1000;
}
