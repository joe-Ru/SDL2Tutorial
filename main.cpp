
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

typedef struct Entity Entity; 

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
static Entity *player;
static SDL_Texture* bulletTexture;
struct App app;
struct Stage stage;
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

static void fireBullet(void)
{
	Entity* bullet;

	bullet = (Entity *)malloc(sizeof(Entity));
	memset(bullet, 0, sizeof(Entity));
	stage.bulletTail->next = bullet;
	stage.bulletTail = bullet;

	bullet->x = player->x;
	bullet->y = player->y;
	bullet->dx = PLAYER_BULLET_SPEED;
	bullet->health = 1;
	bullet->texture = bulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

	bullet->y += (player->h / 2) - (bullet->h / 2);

	player->reload = 8;
}



static void doPlayer(void) {
	player->dx = player->dy = 0;

	if (player->reload > 0)
	{
		player->reload--;
	}

	if (app.keyboard[SDL_SCANCODE_UP])
	{
		player->dy = -PLAYER_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_DOWN])
	{
		player->dy = PLAYER_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_LEFT])
	{
		player->dx = -PLAYER_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_RIGHT])
	{
		player->dx = PLAYER_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0)
	{
		fireBullet();
	}

	player->x += player->dx;
	player->y += player->dy;

}


static void drawPlayer(void) {
	blit(player->texture, player->x, player->y);
}
static void drawBullets(void)
{
	Entity* b;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		blit(b->texture, b->x, b->y);
	}
}

static void draw(void) {
	drawPlayer();
	drawBullets();
}






static void doBullets(void) {
	Entity* b, * prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		b->x += b->dx;
		b->y += b->dy;

		if (b->x > SCREEN_WIDTH)
		{
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}

		prev = b;
	}
}

static void doFighters(void) {
	Entity* e, * prev;

	prev = &stage.fighterHead;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		e->x += e->dx;
		e->y += e->dy;

		if (e != player && e->x < -e->w)
		{
			if (e == stage.fighterTail)
			{
				stage.fighterTail = prev;
			}

			prev->next = e->next;
			free(e);
			e = prev;
		}

		prev = e;
	}

}


static void spawnEnemies(void)
{
	Entity* enemy;

	if (--enemySpawnTimer <= 0)
	{
		enemy = malloc(sizeof(Entity));
		memset(enemy, 0, sizeof(Entity));
		stage.fighterTail->next = enemy;
		stage.fighterTail = enemy;

		enemy->x = SCREEN_WIDTH;
		enemy->y = rand() % SCREEN_HEIGHT;
		enemy->texture = enemyTexture;
		SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

		enemy->dx = -(2 + (rand() % 4));

		enemySpawnTimer = 30 + (rand() % 60);
	}
}


static void drawFighters(void)
{
	Entity* e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		blit(e->texture, e->x, e->y);
	}
}


static void logic(void) {
	doPlayer();

	doFighters();

	doBullets();

	spawnEnemies();
}



static void initPlayer() {
	player = (Entity *)malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity)); //Debug this
	stage.fighterTail->next = player;
	stage.fighterTail = player;

	player->x = 100;
	player->y = 100;

	char fileName[] = "res/gfx/player.png";
	player->texture = loadTexture(fileName);
	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
	std::cout << "End of initPlayer" << std::endl;

}



void initStage(void) {
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;

	initPlayer();

	char fileName[] = "res/gfx/bullet.png";

	bulletTexture = loadTexture(fileName);

	enemySpawnTimer = 0;

}

static void capFrameRate(long* then, float* remainder) {
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1) {
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();

	std::cout << "End of capFrameRate " << std::endl;


}

int main(int argc, char* argv[]) {
	long then;
	float remainder;

	std::cout << &app << std::endl;

	memset(&app, 0, sizeof(App));

	initSDL();

	atexit(SDL_Quit);


	initStage();

	then = SDL_GetTicks();
	
	remainder = 0;

	while (1) {
		prepareScene();

		doInput();

		app.delegate.logic();

		app.delegate.draw();

		prepareScene();

		capFrameRate(&then, &remainder);
	}

	return 0;

}