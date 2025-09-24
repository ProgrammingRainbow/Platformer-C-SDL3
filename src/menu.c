#include "menu.h"
#include "bubble.h"

bool menu_new(struct Menu **menu, SDL_Renderer *renderer, float scale) {
    *menu = calloc(1, sizeof(struct Menu));
    if (!*menu) {
        fprintf(stderr, "Error in calloc of new menu.\n");
        return false;
    }
    struct Menu *m = *menu;

    m->renderer = renderer;
    m->scale = scale;
    m->x = 10;
    m->y = 10;

    SDL_Color color = {127, 64, 64, 255};
    SDL_Color bcolor = {64, 64, 127, 255};

    m->image = bubble_rect(m->renderer, 100, 30, 10, color, bcolor, 0, 5);
    if (!m->image) {
        return false;
    }

    if (!SDL_GetTextureSize(m->image, &m->w, &m->h)) {
        fprintf(stderr, "Error getting Texture size: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_SetTextureScaleMode(m->image, SDL_SCALEMODE_NEAREST)) {
        fprintf(stderr, "Error setting texture scale mode: %s\n",
                SDL_GetError());
        return false;
    }

    return true;
}

void menu_free(struct Menu **menu) {
    if (*menu) {
        struct Menu *m = *menu;

        if (m->image) {
            SDL_DestroyTexture(m->image);
            m->image = NULL;
        }

        m->renderer = NULL;

        free(m);
        m = NULL;
        *menu = NULL;

        printf("Free Menu.\n");
    }
}

void menu_draw(const struct Menu *m, float camera) {
    SDL_FRect rect = {m->x * m->scale + camera, m->y * m->scale,
                      m->w * m->scale, m->h * m->scale};

    SDL_RenderTexture(m->renderer, m->image, NULL, &rect);
}
