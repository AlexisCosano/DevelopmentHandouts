#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "p2DynArray.h"


// TODO 2: done
// ----------------------------------------------------
struct Tileset
{
	SDL_Rect GetTileRect(int id) const;

	const char* name = nullptr;
	SDL_Texture* texture = nullptr;
	int firstgid = 0;
	int tile_width = 0;
	int tile_height = 0;
	int spacing = 0;
	int margin = 0;
	int	tex_width = 0;
	int	tex_height = 0;
	int	num_tiles_width = 0;
	int	num_tiles_height = 0;
};

struct Layer
{
	const char* name = nullptr;
	int width = 0;
	int height = 0;
	uint* data = nullptr;
};

// TODO 1: done
enum Orientation
{
	ORTHOGONAL = 0,
	ISOMETRIC = 1
};

enum RenderOrder
{
	RIGHT_DOWN = 0,
	LEFT_DOWN = 1,
	RIGHT_UP = 2,
	LEFT_UP
};

struct MapNode
{
	Orientation orientation;
	RenderOrder render_order;

	p2List<Tileset*> tilesets;
	p2List<Layer*> layers;

	int width = 0;
	int height = 0;
	int tile_width = 0;
	int tile_height = 0;
	int next_object_id = 1;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:
	bool LoadMapData();
	bool LoadTilesetData(const pugi::xml_node& map_file_tilesetnode, Tileset* tileset_to_load);
	bool LoadTilesetImage(const pugi::xml_node& tileset, Tileset* given_tileset);
	bool LoadLayerData(const pugi::xml_node& map_file_layernode, Layer* layer_to_load);

public:

	// TODO 1: done
	MapNode map_node;
	Tileset tileset;
	Layer layer;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__