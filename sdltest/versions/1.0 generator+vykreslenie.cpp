#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "MazePlan.h"

//Screen dimension constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();
//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);
void plotMaze(MazePlan* maze);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (gWindow == NULL)	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))	{
		printf("Warning: Linear texture filtering not enabled!");
	}
//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return true;
}

bool loadMedia()
{
//Loading success flag
	bool success = true;
//nic zatial neloadujem
	return success;
}


void close()
{
//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
//Destroy renderer
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
//Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
//The final texture
	SDL_Texture* newTexture = NULL;
//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
	//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
	//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

void plotMaze(MazePlan * maze)
{
	int odsadenie = maze->blocksize/2;
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	//draw grid
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);	//yellow
	for (int i = 0; i < maze->cols; i++) {
		for (int j = 0; j < maze->rows; j++) {
			SDL_RenderDrawPoint(gRenderer, j*maze->blocksize+odsadenie, i*maze->blocksize+odsadenie);
		}
	}
	//draw hlines
	//SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
	for (int i = 0;i < maze->rows;i++) {
		for (int j = 0;j < maze->cols-1;j++) {
			if(maze->hlines[i][j]==true)
				SDL_RenderDrawLine(gRenderer, j*maze->blocksize+odsadenie, i*maze->blocksize+odsadenie, (j+1)*maze->blocksize + odsadenie, i*maze->blocksize + odsadenie);
		}
	}
	//draw vlines
	for (int i = 0;i < maze->cols;i++) {
		for (int j = 0;j < maze->rows - 1;j++) {
			if (maze->vlines[i][j] == true)
				SDL_RenderDrawLine(gRenderer, i*maze->blocksize + odsadenie, j*maze->blocksize + odsadenie, i*maze->blocksize + odsadenie, (j+1)*maze->blocksize + odsadenie);
		}
	}
	//draw start point


	//draw end area
	SDL_Rect fillRect = {(maze->endx*maze->blocksize)-(maze->blocksize/4)+odsadenie,(maze->endy*maze->blocksize) - (maze->blocksize / 4)+odsadenie,maze->blocksize/2,maze->blocksize / 2 };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);

	//Update screen
	SDL_RenderPresent(gRenderer);

}

int main(int argc, char ** argv)
{
	//Start up SDL and create window
	if (!init())	{
		printf("Failed to initialize!\n");
		close();
		return 0;
	}
	//Load media
	if (!loadMedia())	{
		printf("Failed to load media!\n");
		close();
		return 0;
	}

	//Main loop flag
	bool quit = false;
	//Event handler
	SDL_Event e;
	struct MazePlan* example = MazePlan::giveExample();
	SDL_SetWindowSize(gWindow, example->blocksize*example->rows, example->blocksize*example->cols);

	// MAIN LOOP
	while (!quit) {
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			} 
			//User presses a key
			else if (e.type == SDL_KEYDOWN)			{
				switch (e.key.keysym.sym)				{
					case SDLK_ESCAPE:
						quit = true;
						break;
				}
			}
		}
		plotMaze(example);
	}

	// CLEANUP
	close();
	return 0;
}