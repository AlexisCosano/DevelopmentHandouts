#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	Layer* layer_to_draw = map_node.layers.start->data;

	for (int y = 0; y < map_node.height; ++y)
	{
		for (int x = 0; x < map_node.width; ++x)
		{
			int tile_id = layer_to_draw->GetXY(x, y);
			if (tile_id > 0)
			{
				Tileset* tileset_to_draw = map_node.tilesets.start->data;

				SDL_Rect tile_rect = tileset_to_draw->GetTileRect(tile_id);
				iPoint world_position = MapToWorldPosition(x, y);

				App->render->Blit(tileset_to_draw->texture, world_position.x, world_position.y, &tile_rect);
			}
		}
	}
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 2: done
	if (map_node.tilesets.start != NULL)
	{
		p2List_item<Tileset*>* iterator = map_node.tilesets.start;

		while(iterator != NULL)
		{
			RELEASE(iterator->data);
			iterator = iterator->next;
		}

		map_node.tilesets.clear();


		p2List_item<Layer*>* iterator2 = map_node.layers.start;

		while (iterator2 != NULL)
		{
			RELEASE(iterator2->data);
			iterator2 = iterator2->next;
		}

		map_node.layers.clear();
	}
	else
	{
		map_node.tilesets.clear();
		map_node.layers.clear();
	}	

	map_file.reset();

	return true;
}

// Load map's data -----------------------------------------------------------------------
bool j1Map::LoadMapData()
{
	pugi::xml_node map_file_mapnode = map_file.child("map");

	if (map_file_mapnode != NULL)
	{
		map_node.width = map_file_mapnode.attribute("width").as_int();
		map_node.height = map_file_mapnode.attribute("height").as_int();

		map_node.tile_width = map_file_mapnode.attribute("tilewidth").as_int();
		map_node.tile_height = map_file_mapnode.attribute("tileheight").as_int();

		map_node.next_object_id = map_file_mapnode.attribute("nextobjectid").as_int();

		const char* orientation_tmp = map_file_mapnode.attribute("orientation").as_string();
		if (orientation_tmp == "orthogonal")
		{
			map_node.orientation = ORTHOGONAL;
		}
		else if (orientation_tmp == "isometric")
		{
			map_node.orientation = ISOMETRIC;
		}

		const char* render_order_tmp = map_file_mapnode.attribute("orientation").as_string();
		if (render_order_tmp == "left-up")
		{
			map_node.render_order = LEFT_UP;
		}
		else if (render_order_tmp == "left-down")
		{
			map_node.render_order = LEFT_DOWN;
		}
		else if (render_order_tmp == "right-up")
		{
			map_node.render_order = RIGHT_UP;
		}
		else if (render_order_tmp == "right-down")
		{
			map_node.render_order = RIGHT_DOWN;
		}

		LOG("Map data successfuly loaded.");
		return(true);
	}
	else
	{
		LOG("Map's data not loaded.");
		return(false);
	}
}

// Get tile's rect
SDL_Rect Tileset::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;

	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));

	return(rect);
}

// Get X and Y from a vector
inline uint Layer::GetXY(int x, int y) const
{
	return(data[x + (y*height)]);
}

// Position from map to world
iPoint j1Map::MapToWorldPosition(int x, int y)
{
	iPoint world_vector;

	if (map_node.orientation == ORTHOGONAL)
	{
		world_vector.x = x * map_node.tile_width;
		world_vector.y = y * map_node.tile_height;
	}
	else
	{
		LOG("This type of map (%s) is not supported.", map_node.orientation);
		world_vector.x = x; 
		world_vector.y = y;
	}

	return (world_vector);
}

// Load tileset's data -----------------------------------------------------------
bool j1Map::LoadTilesetData(const pugi::xml_node& map_file_tilesetnode, Tileset* tileset_to_load)
{
	if (tileset_to_load != NULL)
	{
		tileset_to_load->name = map_file_tilesetnode.attribute("name").as_string();
		tileset_to_load->firstgid = map_file_tilesetnode.attribute("firstgid").as_int();
		tileset_to_load->margin = map_file_tilesetnode.attribute("margin").as_int();
		tileset_to_load->spacing = map_file_tilesetnode.attribute("spacing").as_int();
		tileset_to_load->tile_width = map_file_tilesetnode.attribute("tilewidth").as_int();
		tileset_to_load->tile_height = map_file_tilesetnode.attribute("tileheight").as_int();

		
		LOG("Tileset loaded correctly.");
		return(true);
	}
	else
	{
		LOG("Tileset could not be loaded.");
		return(false);
	}
}

// Load tileset's image
bool j1Map::LoadTilesetImage(const pugi::xml_node& tileset, Tileset* given_tileset)
{
	bool ret = true;
	pugi::xml_node image = tileset.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		given_tileset->texture = App->tex->Load(PATH("maps/", image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(given_tileset->texture, NULL, NULL, &w, &h);
		given_tileset->tex_width = image.attribute("width").as_int();

		if (given_tileset->tex_width <= 0)
		{
			given_tileset->tex_width = w;
		}

		given_tileset->tex_height = image.attribute("height").as_int();

		if (given_tileset->tex_height <= 0)
		{
			given_tileset->tex_height = h;
		}

		given_tileset->num_tiles_width = given_tileset->tex_width / given_tileset->tile_width;
		given_tileset->num_tiles_height = given_tileset->tex_height / given_tileset->tile_height;
	}

	return(ret);
}

// Load layer's data -------------------------------------------------------------
bool j1Map::LoadLayerData(const pugi::xml_node& map_file_layernode, Layer* layer_to_load)
{
	if (layer_to_load != NULL)
	{
		layer_to_load->name = map_file_layernode.attribute("name").as_string();
		layer_to_load->width = map_file_layernode.attribute("width").as_int();
		layer_to_load->height = map_file_layernode.attribute("height").as_int();


		pugi::xml_node layer_data_node = map_file_layernode.child("data");

		layer_to_load->data = new uint[layer_to_load->width * layer_to_load->height];
		memset(layer_to_load->data, 0, layer_to_load->width * layer_to_load->height);

		int i = 0;

		for (pugi::xml_node tile_data_node = layer_data_node.child("tile"); tile_data_node; tile_data_node = tile_data_node.next_sibling("tile"))
		{
			layer_to_load->data[i] = tile_data_node.attribute("gid").as_int();
			LOG("This id is %i", layer_to_load->data[i]);
			i++;
		}

		LOG("Successfully loaded all the layer's data.");
		return(true);
	}
	else
	{
		LOG("There aren't any layers.");
		return(false);
	}
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if(ret == true)
	{
		// TODO 3: done
		ret = LoadMapData();
	}

	if (ret == true)
	{
		// TODO 4: done
		for (pugi::xml_node map_file_tilesetnode = map_file.child("map").child("tileset"); map_file_tilesetnode; map_file_tilesetnode = map_file_tilesetnode.next_sibling("tileset"))
		{
			Tileset* tileset_to_load = new Tileset();

			if (ret == true)
			{
				ret = LoadTilesetData(map_file_tilesetnode, tileset_to_load);
			}

			if (ret == true)
			{
				ret = LoadTilesetImage(map_file_tilesetnode, tileset_to_load);
			}
			
			map_node.tilesets.add(tileset_to_load);
		}
	}
	
	if (ret == true)
	{
		// loading the layers
		for (pugi::xml_node map_file_layernode = map_file.child("map").child("layer"); map_file_layernode; map_file_layernode = map_file_layernode.next_sibling("layer"))
		{
			Layer* layer_to_load = new Layer();

			ret = LoadLayerData(map_file_layernode, layer_to_load);

			if (ret == true)
			{
				map_node.layers.add(layer_to_load);
			}
			else
			{
				LOG("Couldn't load the map's layers.");
			}
		}
	}

	if(ret == true)
	{
		// TODO 5: done
		LOG("The file -%s- has been successfully loaded.", file_name);
		LOG("Width: %i   Height: %i", map_node.width, map_node.height);
		LOG("Tile width: %i    Tile height: %i", map_node.tile_width, map_node.tile_height);
		
		p2List_item<Tileset*>* iterator = map_node.tilesets.start;

		while (iterator != NULL)
		{
			LOG("Tileset ----");
			LOG("Name: %s    Firstgid: %i", iterator->data->name, iterator->data->firstgid);
			LOG("Tile width: %i     Tile height: %i", iterator->data->tile_width, iterator->data->tile_height);
			LOG("Spacing: %i    Margin: %i", iterator->data->spacing, iterator->data->margin);
			iterator = iterator->next;
		}

		p2List_item<Layer*>* iterator2 = map_node.layers.start;

		while (iterator2 != NULL)
		{
			LOG("Layer ----");
			LOG("Name: %s", iterator2->data->name);
			LOG("Width: %i     Height: %i", iterator2->data->width, iterator2->data->height);
			iterator2 = iterator2->next;
		}
	}

	map_loaded = ret;

	return ret;
}

