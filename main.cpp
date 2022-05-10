
#define SDL_MAIN_HANDLED
#include"SDL.h"
#include"SDL_image.h"

#include<iostream>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16

#define MAX_KEYBOARD_KEYS 350

struct Entity {
	float x;
	float y;
	int w;
	int h;
	float dx;
	float dy;
	int health;
	int reload;
	SDL_Texture* texture;
	Entity* next;
};

typedef struct Delegate {
	void (*logic)(void);
	void (*draw)(void);
};


typedef struct App {
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	
};

typedef struct Stage {
	Entity fighterHead, *fighterTail;
	Entity bulletHead, *bulletTail;
};



// ------- ENTITY DECLARATIONS ------- 
struct Entity player;
struct Entity bullet;
struct App app;
// ----------------------------------- 



void initSDL() {


	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't init SDL: %s\n", SDL_GetError());
		exit(1);
	}


	app.window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!app.window) {
		printf("Failed to open %d x %d window: %s\n", 
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());

		exit(1);
	}

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if (!app.renderer) {
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

}


/*
-> operator allows access to structures and unions
used with a pointer variable pointing to a structure or union.

*/

void doKeyDown(SDL_KeyboardEvent* event) {
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
		app.keyboard[event->keysym.scancode] = 1;
	}
}


void(doKeyUp(SDL_KeyboardEvent* event)) {
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS) {
		app.keyboard[event->keysym.scancode] = 0;
	}
}



void doInput(void) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			exit(0);
			break;
		case SDL_KEYDOWN:
			doKeyDown(&event.key);
			break;
		case SDL_KEYUP:
			doKeyUp(&event.key);
			break;
		default:
			break;
		}
	}
}




void prepareScene(void) {
	SDL_SetRenderDrawColor(app.renderer, 96, 128, 255, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene(void) {
	SDL_RenderPresent(app.renderer);
}

SDL_Texture *loadTexture(char *filename) {
	SDL_Texture *texture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
		"Loading %s", filename);

	texture = IMG_LoadTexture(app.renderer, filename);

	return texture;
}

void blit(SDL_Texture* texture, int x, int y) {
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	

	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}


int main(int argc, char* argv[]) {
	
	memset(&app, 0, sizeof(App));
	memset(&player, 0, sizeof(Entity));
	memset(&bullet, 0, sizeof(bullet));


	initSDL();

	player.x = 100;
	player.y = 100;

	char playerSprite[] = "res/gfx/spaceship.png";
	char bulletSprite[] = "res/gfx/bullet.png";

	player.texture = loadTexture(playerSprite);
	bullet.texture = loadTexture(bulletSprite);


	while (1) {
		prepareScene();

		doInput();

		player.x += player.dx;
		player.y += player.dy;

		if (app.up) {
			player.y -= 4;
		}
		if (app.down) {
			player.y += 4;
		}
		if (app.left) {
			player.x -= 4;
		}
		if (app.right) {
			player.x += 4;
		}
		if (app.fire && bullet.health == 0) {
			bullet.x = player.x;
			bullet.y = player.y;
			bullet.dx = 16;
			bullet.dy = 0;
			bullet.health = 1;
		}

		bullet.x += bullet.dx;
		bullet.y += bullet.dy;

		if (bullet.x > SCREEN_WIDTH) {
			bullet.health = 0;
		}

		blit(player.texture, player.x, player.y);

		if (bullet.health > 0) {
			blit(bullet.texture, bullet.x, bullet.y);
		}

		presentScene();

		SDL_Delay(16);
	}

	return 0;

}