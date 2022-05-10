#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include"Entity.hpp"

class RenderWindow
{
public:
	RenderWindow(const char* p_title, int p_w, int p_h);
	SDL_Texture* loadTexture(const char* p_filePath);
	void cleanUp();
	void clear();
	void render(Entity& p_entity); 
	//pass by ref means: do not make a copy instead find it by
	//this mem address
	void display();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
};