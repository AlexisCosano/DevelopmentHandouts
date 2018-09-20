#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"

#include "SDL/include/SDL.h"


j1Window::j1Window() : j1Module()
{
	window = NULL;
	screen_surface = NULL;
	name.create("window");
}

// Destructor
j1Window::~j1Window()
{
}

// Called before render is available
bool j1Window::Awake(pugi::xml_node& module_node)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;

		window_node = &module_node;

		// TODO 4: Done
		if (window_node != nullptr)
		{
			title = window_node->child("title").attribute("window_title").as_string();

			width = window_node->child("width").attribute("window_width").as_int();
			height = window_node->child("height").attribute("window_height").as_int();
			scale = window_node->child("scale").attribute("window_scale").as_int();

			fullscreen = window_node->child("fullscreen").attribute("w_fullscreen").as_bool();
			borderless = window_node->child("borderless").attribute("w_borderless").as_bool();
			resizable = window_node->child("resizable").attribute("w_resizable").as_bool();
			wfullscreen = window_node->child("windowedfullscreen").attribute("w_wfullscreen").as_bool();
		}
		

		if(fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(wfullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(title.GetString(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			
		}
	}

	return ret;
}

// Called before quitting
bool j1Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void j1Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void j1Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

uint j1Window::GetScale() const
{
	return scale;
}