//
//  main.c
//  cosmo-sdl-template (adapted from https://github.com/vkoskiv/c-ray)
//
//  Created by Valtteri Koskivuori on 30/11/2023.
//  Copyright © 2023 Valtteri Koskivuori. All rights reserved.
//

// MIT License

// Copyright (c) 2015, Valtteri Koskivuori

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdbool.h>

#define _COSMO_SOURCE
#include "libc/dlopen/dlfcn.h"

#include "SDL2/SDL_render.h"
#include "SDL2/SDL_events.h"

// TODO: Add all SDL functions
struct sdl_syms {
	void *lib;
	int (*SDL_VideoInit)(const char *driver_name);
	void (*SDL_VideoQuit)(void);
	void (*SDL_Quit)(void);
	void (*SDL_Delay)(uint32_t ms);
	const char *(*SDL_GetError)(void);
	void (*SDL_SetWindowIcon)(SDL_Window *window, SDL_Surface *icon);
	void (*SDL_FreeSurface)(SDL_Surface *surface);
	SDL_Surface *(*SDL_CreateRGBSurfaceFrom)(void *pixels, int width, int height, int depth, int pitch,
											uint32_t rmask, uint32_t gmask, uint32_t bmask, uint32_t amask);
	SDL_Window *(*SDL_CreateWindow)(const char *title, int x, int y, int w, int h, uint32_t flags);
	SDL_Renderer *(*SDL_CreateRenderer)(SDL_Window *window, int index, uint32_t flags);
	SDL_Texture *(*SDL_CreateTexture)(SDL_Renderer *renderer, uint32_t format, int access, int w, int h);
	void (*SDL_DestroyTexture)(SDL_Texture *texture);
	void (*SDL_DestroyRenderer)(SDL_Renderer *renderer);
	void (*SDL_DestroyWindow)(SDL_Window *window);
	void (*SDL_RenderPresent)(SDL_Renderer *renderer);
	int (*SDL_RenderSetLogicalSize)(SDL_Renderer *renderer, int w, int h);
	int (*SDL_SetRenderDrawBlendMode)(SDL_Renderer *renderer, SDL_BlendMode blend_mode);
	int (*SDL_SetRenderDrawColor)(SDL_Renderer *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	int (*SDL_SetTextureBlendMode)(SDL_Texture *texture, SDL_BlendMode blend_mode);
	int (*SDL_RenderSetScale)(SDL_Renderer *renderer, float scaleX, float scaleY);
	int (*SDL_RenderClear)(SDL_Renderer *renderer);
	int (*SDL_PollEvent)(SDL_Event *event);
	int (*SDL_UpdateTexture)(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch);
	int (*SDL_RenderCopy)(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect);
	int (*SDL_UpdateWindowSurface)(SDL_Window *window);
};

static void *try_find_sdl2_lib(void) {
	char *candidates[] = {
		"libSDL2-2.0.so",
		"libSDL2-2.0.0.dylib",
		"SDL2.dll"
	};
	void *lib = NULL;
	for (size_t i = 0; i < (sizeof(candidates) / sizeof(*candidates)); ++i) {
		if ((lib = cosmo_dlopen(candidates[i], RTLD_LAZY))) return lib;
	}

	printf("Couldn't find SDL library (%s), tried the following names: ", cosmo_dlerror());
	for (size_t i = 0; i < (sizeof(candidates) / sizeof(*candidates)); ++i) {
		printf("\"%s\" ", candidates[i]);
	}
	printf("\n");

	return NULL;
}

static struct sdl_syms *try_get_sdl2_syms(void) {
	void *sdl2 = try_find_sdl2_lib();
	if (!sdl2) return NULL;
	struct sdl_syms *syms = calloc(1, sizeof(*syms));
	*syms = (struct sdl_syms){
		.lib = sdl2,
		.SDL_VideoInit              = cosmo_dlsym(sdl2, "SDL_VideoInit"),
		.SDL_VideoQuit              = cosmo_dlsym(sdl2, "SDL_VideoQuit"),
		.SDL_Quit                   = cosmo_dlsym(sdl2, "SDL_Quit"),
		.SDL_Delay                  = cosmo_dlsym(sdl2, "SDL_Delay"),
		.SDL_GetError               = cosmo_dlsym(sdl2, "SDL_GetError"),
		.SDL_SetWindowIcon          = cosmo_dlsym(sdl2, "SDL_SetWindowIcon"),
		.SDL_FreeSurface            = cosmo_dlsym(sdl2, "SDL_FreeSurface"),
		.SDL_CreateRGBSurfaceFrom   = cosmo_dlsym(sdl2, "SDL_CreateRGBSurfaceFrom"),
		.SDL_CreateWindow           = cosmo_dlsym(sdl2, "SDL_CreateWindow"),
		.SDL_CreateRenderer         = cosmo_dlsym(sdl2, "SDL_CreateRenderer"),
		.SDL_CreateTexture          = cosmo_dlsym(sdl2, "SDL_CreateTexture"),
		.SDL_DestroyTexture         = cosmo_dlsym(sdl2, "SDL_DestroyTexture"),
		.SDL_DestroyRenderer        = cosmo_dlsym(sdl2, "SDL_DestroyRenderer"),
		.SDL_DestroyWindow          = cosmo_dlsym(sdl2, "SDL_DestroyWindow"),
		.SDL_RenderPresent          = cosmo_dlsym(sdl2, "SDL_RenderPresent"),
		.SDL_RenderSetLogicalSize   = cosmo_dlsym(sdl2, "SDL_RenderSetLogicalSize"),
		.SDL_SetRenderDrawBlendMode = cosmo_dlsym(sdl2, "SDL_SetRenderDrawBlendMode"),
		.SDL_SetRenderDrawColor     = cosmo_dlsym(sdl2, "SDL_SetRenderDrawColor"),
		.SDL_SetTextureBlendMode    = cosmo_dlsym(sdl2, "SDL_SetTextureBlendMode"),
		.SDL_RenderSetScale         = cosmo_dlsym(sdl2, "SDL_RenderSetScale"),
		.SDL_RenderClear            = cosmo_dlsym(sdl2, "SDL_RenderClear"),
		.SDL_PollEvent              = cosmo_dlsym(sdl2, "SDL_PollEvent"),
		.SDL_UpdateTexture          = cosmo_dlsym(sdl2, "SDL_UpdateTexture"),
		.SDL_RenderCopy             = cosmo_dlsym(sdl2, "SDL_RenderCopy"),
		.SDL_UpdateWindowSurface    = cosmo_dlsym(sdl2, "SDL_UpdateWindowSurface"),
	};
	// Quick check to make sure they were all found
	for (size_t i = 0; i < (sizeof(struct sdl_syms) / sizeof(void *)); ++i) {
		if (!((void **)syms)[i]) {
			printf("sdl_syms[%zu] is NULL, check for typos\n", i);
			free(syms);
			return NULL;
		}
	}
	return syms;
}

struct color {
	float r, g, b, a;
};

// It's not obvious to me what p, q and t are supposed to be short for, copied here verbatim.
static float hue_to_rgb(float p, float q, float t) {
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;
	if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f) return q;
	if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
	return p;
}

static struct color color_from_hsl(float hue, float saturation, float lightness) {
	// Map these to HSL standard ranges. 0-360 for h, 0-100 for s and l
	hue = hue / 360.0f;
	saturation = saturation / 100.0f;
	lightness = lightness / 100.0f;
	if (saturation == 0.0f) {
		return (struct color){ lightness, lightness, lightness, 1.0f };
	} else {
		const float q = lightness < 0.5f ? lightness * (1.0f + saturation) : lightness + saturation - lightness * saturation;
		const float p = 2.0f * lightness - q;
		return (struct color){ hue_to_rgb(p, q, hue + 1.0f / 3.0f), hue_to_rgb(p, q, hue), hue_to_rgb(p, q, hue - 1.0f / 3.0f), 1.0f };
	}
}

#define WIDTH 1280
#define HEIGHT 800
int main(void) {
	struct sdl_syms *sym = try_get_sdl2_syms();
	if (!sym) {
		printf("Failed to get symbols, bailing out.\n");
		return -1;
	}

	
	if (sym->SDL_VideoInit(NULL) < 0) {
		printf("SDL couldn't initialize, error: \"%s\"\n", sym->SDL_GetError());
		return -1;
	}

	SDL_WindowFlags flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
	SDL_Window *window = sym->SDL_CreateWindow("cosmo-sdl-template © Valtteri Koskivuori 2023, press Q or click on window to exit",
								SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED,
								WIDTH,
								HEIGHT,
								flags);
	if (!window) {
		printf("Window couldn't be created, error: \"%s\"\n", sym->SDL_GetError());
		return -1;
	}

	SDL_Renderer *renderer = sym->SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	// Try a software fallback before bailing out
	if (!renderer) renderer = sym->SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer) {
		printf("Renderer couldn't be created, error: \"%s\"\n", sym->SDL_GetError());
		return -1;
	}
	sym->SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
	sym->SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	sym->SDL_RenderSetScale(renderer, 1.0f, 1.0f);
	
	float hue = 0.0f;
	bool running = true;
	while (running) {
		SDL_Event event = { 0 };
		while (sym->SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
				if (event.key.keysym.sym == SDLK_q) {
					running = false;
				}
			}
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}
		
		hue += 0.5f;
		if (hue >= 360.0f) hue = 0.0f;
		struct color c = color_from_hsl(hue, 100, 50);
		sym->SDL_SetRenderDrawColor(renderer, c.r * 255, c.g * 255, c.b * 255, 128);
		sym->SDL_RenderClear(renderer);
		sym->SDL_RenderPresent(renderer);
		sym->SDL_UpdateWindowSurface(window);
		sym->SDL_Delay(16);
	}

	sym->SDL_DestroyRenderer(renderer);
	sym->SDL_DestroyWindow(window);
	sym->SDL_VideoQuit();
	sym->SDL_Quit();
	cosmo_dlclose(sym->lib);
	free(sym);
	
	return 0;
}
