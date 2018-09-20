#ifndef __j1WINDOW_H__
#define __j1WINDOW_H__

#include "j1Module.h"
#include "PugiXml\src\pugixml.hpp"

struct SDL_Window;
struct SDL_Surface;

class j1Window : public j1Module
{
public:

	j1Window();

	// Destructor
	virtual ~j1Window();

	// Called before render is available
	bool Awake(pugi::xml_node& module_node);

	// Called before quitting
	bool CleanUp();

	// Set window's title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	uint GetScale() const;

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
	

private:
	p2SString	title = "ERROR: Configuration not loaded.";
	uint		width = 1024;
	uint		height = 768;
	uint		scale = 1;

	bool        fullscreen = false;
	bool        borderless = false;
	bool        resizable = false;
	bool        wfullscreen = false;

	pugi::xml_node* window_node;
};

#endif // __j1WINDOW_H__