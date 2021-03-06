#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "PugiXml\src\pugixml.hpp"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node& module_node);

	// Update
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Save & load
	bool Save();
	bool Load();

	// Save audio
	void SaveVolume();

public:

	uint fx_min_volume;
	uint fx_max_volume;

	uint music_min_volume;
	uint music_max_volume;

	uint music_volume;
	uint fx_volume;

private:

	_Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;

	pugi::xml_node* audio_node = nullptr;
};

#endif // __j1AUDIO_H__