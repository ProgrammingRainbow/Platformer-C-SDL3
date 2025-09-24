#include "bubble.h"
#include <SDL3/SDL_surface.h>

void blit_symmetric_points(SDL_Surface *src_surf, SDL_Surface *target_surf,
                           int radius, int x, int y);
SDL_Surface *solid_color_surface(SDL_Surface *src_surf, SDL_Color color);

void blit_symmetric_points(SDL_Surface *src_surf, SDL_Surface *target_surf,
                           int radius, int x, int y) {
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + x, radius + y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + x, radius - y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - x, radius + y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - x, radius - y, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + y, radius + x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius + y, radius - x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - y, radius + x, src_surf->w, src_surf->h});
    SDL_BlitSurface(
        src_surf, NULL, target_surf,
        &(SDL_Rect){radius - y, radius - x, src_surf->w, src_surf->h});
}

SDL_Surface *solid_color_surface(SDL_Surface *src_surf, SDL_Color color) {
    if (!src_surf) {
        fprintf(stderr, "Error: Surface is NULL.\n");
        return NULL;
    }

    SDL_Surface *color_surf = SDL_DuplicateSurface(src_surf);
    if (!color_surf) {
        fprintf(stderr, "Error copying Surface: %s\n", SDL_GetError());
        return NULL;
    }

    if (!SDL_LockSurface(color_surf)) {
        fprintf(stderr, "Error locking Surface: %s\n", SDL_GetError());
        SDL_DestroySurface(color_surf);
        return NULL;
    }

    int width = color_surf->w;
    int height = color_surf->h;

    const SDL_PixelFormatDetails *format_details =
        SDL_GetPixelFormatDetails(color_surf->format);
    if (!format_details) {
        fprintf(stderr, "Error getting format details: %s\n", SDL_GetError());
        SDL_DestroySurface(color_surf);
        return NULL;
    }

    if (format_details->bits_per_pixel != 32) {
        fprintf(stderr, "Error not a 32bit Surface.\n");
        SDL_DestroySurface(color_surf);
        return NULL;
    }

    Uint32 *pixels = (Uint32 *)color_surf->pixels;
    SDL_Palette *palette = SDL_GetSurfacePalette(color_surf);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint32 pixel = pixels[width * y + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, format_details, palette, &r, &g, &b, &a);
            if (a > 0) {
                pixel = SDL_MapRGBA(format_details, palette, color.r, color.g,
                                    color.b, a);
                pixels[width * y + x] = pixel;
            }
        }
    }

    SDL_UnlockSurface(color_surf);

    return color_surf;
}

SDL_Surface *bubble_surface(SDL_Surface *src_surf, int radius,
                            SDL_Color outer_color) {
    if (!src_surf) {
        fprintf(stderr, "Error: Surface is NULL.\n");
        return NULL;
    }

    int width = src_surf->w + radius * 2;
    int height = src_surf->h + radius * 2;

    SDL_Surface *target_surf =
        SDL_CreateSurface(width, height, src_surf->format);
    if (!target_surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface *outer_surf = solid_color_surface(src_surf, outer_color);
    if (!outer_surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        SDL_DestroySurface(target_surf);
        return NULL;
    }

    // // Polar Coordinates Trigonometry Algorithm
    // for (int index = 0; index < (2 * M_PI * radius); index++) {
    //     double rad = (double)index / radius;
    //     int x = (int)(cos(rad) * radius) + radius;
    //     int y = (int)(sin(rad) * radius) + radius;
    //
    //     SDL_BlitSurface(outer_surf, NULL, target_surf,
    //                     &(SDL_Rect){x, y, outer_surf->w, outer_surf->h});
    // }

    // Bresenham's Circle Drawing Algorithm
    // https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    blit_symmetric_points(outer_surf, target_surf, radius, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        blit_symmetric_points(outer_surf, target_surf, radius, x, y);
    }

    SDL_BlitSurface(src_surf, NULL, target_surf,
                    &(SDL_Rect){radius, radius, src_surf->w, src_surf->h});

    SDL_DestroySurface(outer_surf);

    return target_surf;
}

SDL_Surface *bubble_create_text(const char *str, float size, int radius,
                                SDL_Color inner_color, SDL_Color outer_color) {
    TTF_Font *font = TTF_OpenFont("fonts/freesansbold.ttf", size);
    if (!font) {
        fprintf(stderr, "Error Opening Font: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Surface *text_surf = TTF_RenderText_Blended(font, str, 0, inner_color);
    TTF_CloseFont(font);
    font = NULL;
    if (!text_surf) {
        fprintf(stderr, "Error rendering text to Surface: %s\n",
                SDL_GetError());

        return NULL;
    }

    SDL_Surface *bubble_surf = bubble_surface(text_surf, radius, outer_color);
    SDL_DestroySurface(text_surf);
    text_surf = NULL;
    if (!bubble_surf) {
        return NULL;
    }

    return bubble_surf;
}

SDL_Texture *bubble_rect(SDL_Renderer *renderer, int w, int h, int radius,
                         SDL_Color color, SDL_Color bcolor, int border,
                         int shadow) {

    SDL_Surface *surf = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB32);
    if (!surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        return NULL;
    }

    Uint32 surf_color = SDL_MapSurfaceRGBA(surf, 0, 0, 0, 255);

    if (!SDL_FillSurfaceRect(surf, NULL, surf_color)) {
        fprintf(stderr, "Error creating Surface Rect: %s\n", SDL_GetError());
        SDL_DestroySurface(surf);
        return NULL;
    }

    // SDL_SetSurfaceAlphaMod(surf, 127);

    int width = (radius + border) * 2 + w + shadow;
    int height = (radius + border) * 2 + h + shadow;

    SDL_Surface *shadow_surf =
        SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB32);
    if (!shadow_surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        SDL_DestroySurface(surf);
        return NULL;
    }

    // Bresenham's Circle Drawing Algorithm
    // https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm
    int x = 0;
    int y = radius + border;
    int d = 3 - 2 * (radius + border);
    blit_symmetric_points(surf, shadow_surf, radius + border + shadow, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        blit_symmetric_points(surf, shadow_surf, radius + border + shadow, x,
                              y);
    }

    SDL_Surface *target_surf =
        SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB32);
    if (!target_surf) {
        fprintf(stderr, "Error creating new Surface: %s\n", SDL_GetError());
        SDL_DestroySurface(surf);
        SDL_DestroySurface(shadow_surf);
        return NULL;
    }

    SDL_SetSurfaceAlphaMod(shadow_surf, 127);
    SDL_BlitSurface(shadow_surf, NULL, target_surf, NULL);
    SDL_DestroySurface(shadow_surf);
    shadow_surf = NULL;

    if (border > 0) {
        surf_color =
            SDL_MapSurfaceRGBA(surf, bcolor.r, bcolor.g, bcolor.b, bcolor.a);

        if (!SDL_FillSurfaceRect(surf, NULL, surf_color)) {
            fprintf(stderr, "Error creating Surface Rect: %s\n",
                    SDL_GetError());
            SDL_DestroySurface(surf);
            return NULL;
        }

        x = 0;
        y = radius + border;
        d = 3 - 2 * radius + border;
        blit_symmetric_points(surf, target_surf, radius + border, x, y);
        while (y >= x) {
            x++;
            if (d > 0) {
                y--;
                d = d + 4 * (x - y) + 10;
            } else {
                d = d + 4 * x + 6;
            }
            blit_symmetric_points(surf, target_surf, radius + border, x, y);
        }
    }

    surf_color = SDL_MapSurfaceRGBA(surf, color.r, color.g, color.b, color.a);

    if (!SDL_FillSurfaceRect(surf, NULL, surf_color)) {
        fprintf(stderr, "Error creating Surface Rect: %s\n", SDL_GetError());
        SDL_DestroySurface(surf);
        return NULL;
    }

    x = 0;
    y = radius;
    d = 3 - 2 * radius;
    blit_symmetric_points(surf, target_surf, radius + border, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        blit_symmetric_points(surf, target_surf, radius + border, x, y);
    }

    SDL_DestroySurface(surf);
    surf = NULL;

    SDL_Texture *image = SDL_CreateTextureFromSurface(renderer, target_surf);
    SDL_DestroySurface(target_surf);
    target_surf = NULL;
    if (!image) {
        fprintf(stderr, "Error creating Texture from Surface: %s\n",
                SDL_GetError());
        return NULL;
    }

    return image;
}
