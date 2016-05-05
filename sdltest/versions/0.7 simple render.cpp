#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 480;

//Key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

//Starts up SDL and creates window
	bool init();
//Loads media
	bool loadMedia();
//Frees media and shuts down SDL
	void close();
//Loads individual image
	SDL_Surface* loadSurface(std::string path);
//Loads individual image as texture
	SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
	SDL_Window* gWindow = NULL;
//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;
//The images that correspond to a keypress
	SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//Current displayed image
	SDL_Surface* gCurrentSurface = NULL;
//Stretched image
	SDL_Surface* gStretchedSurface = NULL;
//PNG image
	SDL_Surface* gPNGSurface = NULL;

//The window renderer
	SDL_Renderer* gRenderer = NULL;
//Current displayed texture
	SDL_Texture* gTexture = NULL;

bool init()
{
//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
//Create window
	gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}
//Create renderer for window
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
//Get window surface
	gScreenSurface = SDL_GetWindowSurface(gWindow);

	return true;
}

bool loadMedia()
{
//Loading success flag
	bool success = true;
//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("press.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}
//Load up surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("up.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("Failed to load up image!\n");
		success = false;
	}
//Load down surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("down.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("Failed to load down image!\n");
		success = false;
	}
//Load left surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("left.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("Failed to load left image!\n");
		success = false;
	}
//Load right surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("right.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
	{
		printf("Failed to load right image!\n");
		success = false;
	}
//Load stretching surface
	gStretchedSurface = loadSurface("stretch.bmp");
	if (gStretchedSurface == NULL)
	{
		printf("Failed to load stretching image!\n");
		success = false;
	}
//Load PNG surface
	gPNGSurface = loadSurface("loaded.png");
	if (gPNGSurface == NULL)
	{
		printf("Failed to load PNG image!\n");
		success = false;
	}
//Load PNG texture
	gTexture = loadTexture("texture.png");
	if (gTexture == NULL)
	{
		printf("Failed to load texture image!\n");
		success = false;
	}
	return success;
}


void close()
{
//Deallocate surfaces
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}
//Free loaded image
	SDL_FreeSurface(gStretchedSurface);
	gStretchedSurface = NULL;
//Free loaded texture
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
//Destroy renderer
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;
	//Load image at specified path
	//SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return NULL;
	}

	//Convert surface to screen format
	optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
	if (optimizedSurface == NULL)
	{
		printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	
	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	return optimizedSurface;
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

int main(int argc, char ** argv)
{
//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
		close();
		return 0;
	}
//Load media
	if (!loadMedia())
	{
		printf("Failed to load media!\n");
		close();
		return 0;
	}

//Main loop flag
	bool quit = false;
//Event handler
	SDL_Event e;
//Set default current surface
	gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

// MAIN LOOP
// stlacenie 'r' aktivuje render, 't' deaktivuje
	bool render = false;
	while (!quit) {		
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT)	{ 
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				if (render) {
					if (e.key.keysym.sym == SDLK_t) {
						render = false;
					}
				}
				else {
					//Select surfaces based on key press
					switch (e.key.keysym.sym) {
					case SDLK_UP:
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
						break;
					case SDLK_DOWN:
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
						break;
					case SDLK_LEFT:
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
						break;
					case SDLK_RIGHT:
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
						break;
					case SDLK_s:
						gCurrentSurface = gStretchedSurface;
						break;
					case SDLK_d:
						gCurrentSurface = gPNGSurface;
						break;
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_r:
						render = true;
						break;
					default:
						gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
						break;
					}
				}			
			}
		}

		if (render) {
			//Clear screen
			SDL_RenderClear(gRenderer);
			//Render texture to screen
			SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
			//Update screen
			SDL_RenderPresent(gRenderer);
		} 
		else {
			//Apply the image stretched
			SDL_Rect stretchRect;
			stretchRect.x = 0;
			stretchRect.y = 0;
			stretchRect.w = SCREEN_WIDTH;
			stretchRect.h = SCREEN_HEIGHT;
			SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);

			//Update the surface
			SDL_UpdateWindowSurface(gWindow);
		}
	}

// CLEANUP
	close();
	return 0;
}