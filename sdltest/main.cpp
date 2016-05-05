#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "MazeSimulator.h"
#include "MouseV1.h"

#define INTERACTIVE
#define DEMO			//odkomentuj ak chces demonstrativne 5x5 bludisko (fixny layout, nemeni sa - vhodne na testovanie robotov)
//#define FULLSCREEN	//odkomentuj ak chces fullscreen, zakomentuj ak windowed... !! WARNING - nedavaj debug pokial si vo fullscreen !!
//#define COLORS

#define BACKGROUND_COLOR 0x00, 0x00, 0x00, 0xFF		//white
#define LINE_COLOR 0xFF, 0xFF, 0xFF, 0xFF			//black
#define ODSADENIE 0	//pridavne odsadenie od okraja - ak nie je definovane, tak je blocksize/2

#ifdef FULLSCREEN
	#define MAZE_ATTRIBUTES 16,9,120,15		//cols,rows,blocksize,lineWidth
	#define MOUSE_POS 15,8,0				//optimalizovane pre 1920x1080
#else 
	#define MAZE_ATTRIBUTES 10,5,100,15
	#define MOUSE_POS 1,1,0
#endif

#ifdef COLORS
	#define POINT_COLOR_STRAIGHT	0x00, 0xFF, 0x00, 0xFF		//c_straight
	#define POINT_COLOR_FSTRAIGHT	0x00, 0xFF, 0x00, 0xFF		//f_straight- rovna vlavo vpravo
	#define POINT_COLOR_RIGHT		0x00, 0x00, 0xFF, 0xFF		//c_right	- aktualne vpravo a dole
	#define POINT_COLOR_LEFT		0x00, 0x00, 0xFF, 0xFF		//c_left	- aktualne vlavo a dole
	#define POINT_COLOR_FLEFT		0x00, 0x00, 0xFF, 0xFF		//f_left	- aktualne vlavo a hore
	#define POINT_COLOR_FRIGHT		0x00, 0x00, 0xFF, 0xFF		//f_right	- aktualne vpravo a hore
	
	#define POINT_COLOR_CROSS		0xFF, 0x00, 0x00, 0xFF		//c_cross
	#define POINT_COLOR_TJUN		0xFF, 0xFF, 0x00, 0xFF		//c_tjun
	#define POINT_COLOR_TRIGHT		0xFF, 0xFF, 0x00, 0xFF		//c_tleft
	#define POINT_COLOR_TLEFT		0xFF, 0xFF, 0x00, 0xFF		//c_tright
	#define POINT_COLOR_FTJUN		0xFF, 0xFF, 0x00, 0xFF		//f_tjun	- tckova krizovatka hore,vlavo,vpravo 

	#define POINT_COLOR_FINISH		0xFF, 0xFF, 0xFF, 0xFF		//c_finish
	#define POINT_COLOR_DEADEND		0xFF, 0x00, 0xFF, 0xFF		//c_deadend alebo f_deadend
#endif

int lastAngle = 0;	//na vykreslenie otacania
Color** gridpointColors = NULL;

bool init();	//Starts up SDL and creates window
bool loadMedia();	//Loads media
void close();	//Frees media and shuts down SDL
SDL_Texture* loadTexture(std::string path);	//Loads individual image as texture
void plotMaze(MazePlan* maze);	//vykreslenie bludiska

#ifdef COLORS
Color** createJunctionColors(MazeSimulator* sim);	//vrati pole farieb rows*cols, vyrata z mazeplanu, farby urci podla defines POINT_COLOR_x
void deleteJunctionColors(Color** junctionColors, MazePlan* maze);	//deletne pole farieb, z mazeplanu zoberie velkost 
#endif

SDL_Window* gWindow = NULL;	//The window we'll be rendering to
SDL_Renderer* gRenderer = NULL;	//The window renderer
SDL_Texture* tankTexture = NULL;	//tank texture

bool init()
{
//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
//Create window
#ifdef FULLSCREEN
	gWindow = SDL_CreateWindow("Bludisko", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	gWindow = SDL_CreateWindow("Bludisko", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
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
//Load PNG texture
	tankTexture = loadTexture("Tank-GTA2.png");
	if (tankTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	return success;
}

void close()
{
//Destroy texture
	SDL_DestroyTexture(tankTexture);
	tankTexture = NULL;
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
	int odsadenie = maze->blocksize/2+ODSADENIE;
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, BACKGROUND_COLOR);
	SDL_RenderClear(gRenderer);

	//draw grid
	SDL_SetRenderDrawColor(gRenderer, LINE_COLOR);	//
	/*for (int i = 0; i < maze->rows; i++) {
		for (int j = 0; j < maze->cols; j++) {
			SDL_Rect fillRect = { (j*maze->blocksize + odsadenie)-maze->lineWidth/2, (i*maze->blocksize + odsadenie) - maze->lineWidth / 2, 
				maze->lineWidth, maze->lineWidth };
			SDL_RenderFillRect(gRenderer, &fillRect);
		}
	}*/
	//draw hlines
	for (int i = 0;i < maze->rows;i++) {
		for (int j = 0;j < maze->cols;j++) {
			if(maze->hlines[i][j] == true)
			{
				SDL_Rect fillRect = { j*maze->blocksize + odsadenie, (i*maze->blocksize + odsadenie)-maze->lineWidth/2, maze->blocksize, maze->lineWidth };
				SDL_RenderFillRect(gRenderer, &fillRect);
			}
		}
	}
	//draw vlines
	for (int i = 0;i < maze->cols;i++) {
		for (int j = 0;j < maze->rows;j++) {
			if (maze->vlines[i][j] == true)
			{
				SDL_Rect fillRect = { (i*maze->blocksize + odsadenie)-maze->lineWidth/2, j*maze->blocksize + odsadenie, maze->lineWidth, maze->blocksize };
				SDL_RenderFillRect(gRenderer, &fillRect);
			}
		}
	}

	//draw grid - na kazdom policku vykresli stvorcek, farbu vytiahne z pola, ktore sa naplnilo predtym
	for (int i = 0; i < maze->rows; i++) {
		for (int j = 0; j < maze->cols; j++) {
		#ifdef COLORS
			SDL_SetRenderDrawColor(gRenderer, gridpointColors[i][j].r, gridpointColors[i][j].g, gridpointColors[i][j].b, gridpointColors[i][j].a);
		#endif
			SDL_Rect fillRect = { (j*maze->blocksize + odsadenie) - maze->lineWidth / 2, (i*maze->blocksize + odsadenie) - maze->lineWidth / 2,
				maze->lineWidth, maze->lineWidth };
			SDL_RenderFillRect(gRenderer, &fillRect);
		}
	}
	//draw start point

	//draw end area
	SDL_Rect fillRect = {(maze->endx*maze->blocksize)-(maze->blocksize/4)+odsadenie,(maze->endy*maze->blocksize) - (maze->blocksize / 4)+odsadenie,maze->blocksize/2,maze->blocksize / 2 };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &fillRect);

	//Update screen
	//SDL_RenderPresent(gRenderer);
}

void plotRobot(MazePlan * maze, Position pos) {
	printf("pos x:%f y:%f angle:%f\n", pos.x, pos.y, pos.angle);

	int odsadenie = maze->blocksize / 2 + ODSADENIE;
	int realX = pos.x*maze->blocksize + odsadenie;
	int realY = pos.y*maze->blocksize + odsadenie;
	SDL_Rect fillRect = { realX-maze->blocksize/4, realY - maze->blocksize / 4, maze->blocksize/2, maze->blocksize / 2 };
	//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
	//SDL_RenderFillRect(gRenderer, &fillRect);

	//otocenie a vykrslenie textury, +90 lebo tank.png je otocene dolava
	SDL_RenderCopyEx(gRenderer, tankTexture, NULL, &fillRect, pos.angle+90, NULL, SDL_FLIP_NONE);

	//Update screen
	//SDL_RenderPresent(gRenderer);
}

#ifdef COLORS
Color** createJunctionColors(MazeSimulator* sim) {
	MazePlan* maze = sim->getMazePlan();
	gridpointColors = new Color*[maze->rows]();
	for (int i = 0; i < maze->rows; i++)
		gridpointColors[i] = new Color[maze->cols]();

	Position pos = { 0,0,0 };
	CellType ct;
	for (int i = 0; i < maze->cols; i++) {
		pos.x = i;
		for (int j = 0; j < maze->rows; j++) {
			//ziska CellType, prevedie ho do farby, ulozi farbu
			pos.y = j;
			ct = sim->giveCellType(pos);
			switch (ct) {
			case CellType::c_straight:
				gridpointColors[j][i] = { POINT_COLOR_STRAIGHT };
				break;
			case CellType::c_right:
				gridpointColors[j][i] = { POINT_COLOR_RIGHT };
				break;
			case CellType::c_left:
				gridpointColors[j][i] = { POINT_COLOR_LEFT };
				break;
			case CellType::c_cross:
				gridpointColors[j][i] = { POINT_COLOR_CROSS };
				break;
			case CellType::c_tjun:
				gridpointColors[j][i] = { POINT_COLOR_TJUN };
				break;
			case CellType::c_tleft:
				gridpointColors[j][i] = { POINT_COLOR_TLEFT };
				break;
			case CellType::c_tright:
				gridpointColors[j][i] = { POINT_COLOR_TRIGHT };
				break;
			case CellType::c_deadend:
				gridpointColors[j][i] = { POINT_COLOR_DEADEND };
				break;
			case CellType::f_deadend:
				gridpointColors[j][i] = { POINT_COLOR_DEADEND };
				break;
			case CellType::f_left:
				gridpointColors[j][i] = { POINT_COLOR_FLEFT };
				break;
			case CellType::f_right:
				gridpointColors[j][i] = { POINT_COLOR_FRIGHT };
				break;
			case CellType::f_tjun:
				gridpointColors[j][i] = { POINT_COLOR_FTJUN };
				break;
			case CellType::f_straight:
				gridpointColors[j][i] = { POINT_COLOR_FSTRAIGHT };
				break;
			case CellType::c_finish:
				gridpointColors[j][i] = { POINT_COLOR_FINISH };
			default:
				gridpointColors[j][i] = { 0xFF,0x00,0x00,0xFF };
			}
		}
	}
	return gridpointColors;
}

void deleteJunctionColors(Color ** junctionColors, MazePlan * maze)
{
	for (int i = 0; i < maze->rows; i++)
		delete[] junctionColors[i];
	delete[] junctionColors;
}
#endif

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

	//vytvorenie robota
	MouseV1* mouse = new MouseV1();
	Position mouse_pos = { MOUSE_POS };
	//vytvorenie simulatora 
#ifdef DEMO
	MazeSimulator* sim = new MazeSimulator(mouse, mouse_pos, true);
#else
	MazeAttributes mattr = {MAZE_ATTRIBUTES};
	MazeSimulator* sim = new MazeSimulator(mouse,mouse_pos, mattr);
#endif

#ifndef FULLSCREEN
	//nastavenie velkosti okna
	int w, h = 0;
	sim->getWindowSize(&w,&h);
	SDL_SetWindowSize(gWindow, w, h);
#endif

#ifdef COLORS
	createJunctionColors(sim);
#endif
	//uvodne vykreslenie
	plotMaze(sim->getMazePlan());		//vykreslenie bludiska
	plotRobot(sim->getMazePlan(), mouse_pos);		//vykreslenie robota

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
					case SDLK_SPACE:
				#ifndef INTERACTIVE
							//ziskanie novej lokacie robota
							mouse_pos = sim->updateRobotPos(false);
				#endif
						break;

				#ifdef INTERACTIVE
					case SDLK_DOWN: mouse_pos = sim->updateRobotPos('S'); break;
					case SDLK_UP: mouse_pos = sim->updateRobotPos('N'); break;
					case SDLK_LEFT:	mouse_pos = sim->updateRobotPos('W'); break;
					case SDLK_RIGHT: mouse_pos = sim->updateRobotPos('E'); break;
				#endif
				}
			}
		}
		//ziskanie novej lokacie robota
#ifdef INTERACTIVE
		mouse_pos = sim->updateRobotPos('0');
#else 
		mouse_pos = sim->updateRobotPos(true);
#endif		
		//vykreslenie bludiska
		plotMaze(sim->getMazePlan());
		//vykreslenie robota
		plotRobot(sim->getMazePlan(), mouse_pos);
		//Update screen
		SDL_RenderPresent(gRenderer);
		SDL_Delay(16);
	}

	// CLEANUP
#ifdef COLORS
	deleteJunctionColors(gridpointColors,sim->getMazePlan());
#endif
	delete mouse;
	delete sim;
	close();
	return 0;
}