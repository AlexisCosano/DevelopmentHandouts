#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	img = App->tex->Load("textures/test.png");
	App->audio->PlayMusic("audio/music/music_sadpiano.ogg");
	App->map->Load("maps/hello2.tmx");
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// TODO 2(2)
	// Save & Load ------------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->CallSave();

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->CallLoad();

	// Move camera ------------------------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y += 1;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x -= 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x += 1;

	//  Control volume --------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_REPEAT)
	{
		if (App->audio->music_volume >= App->audio->music_min_volume && App->audio->music_volume <= App->audio->music_max_volume - 1)
		{
			App->audio->music_volume++;
			LOG("Volume is now: %i", App->audio->music_volume);
		}
	}
		
	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_REPEAT)
	{
		if (App->audio->music_volume >= App->audio->music_min_volume + 1 && App->audio->music_volume <= App->audio->music_max_volume)
		{
			App->audio->music_volume--;
			LOG("Volume is now: %i", App->audio->music_volume);
		}
	}

	//App->render->Blit(img, 0, 0);

	// TODO 7: done
	App->map->Draw();

	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Layers:%d",
		App->map->map_node.width, App->map->map_node.height,
		App->map->map_node.tile_width, App->map->map_node.tile_height,
		App->map->map_node.tilesets.count(), App->map->map_node.layers.count());

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
