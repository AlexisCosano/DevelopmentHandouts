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

	// TODO 6: done
	p2List_item<Tileset*>* iterator = map_node.tilesets.start;

	while(iterator != NULL)
	{
		App->render->Blit(iterator->data->texture, 0, 0);
		iterator = iterator->next;
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
	}
	else
	{
		map_node.tilesets.clear();
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

// Load tileset's data -----------------------------------------------------------
bool j1Map::LoadTilesetData(const pugi::xml_node& map_file_tilesetnode, Tileset* tileset_to_load)
{
	if (tileset_to_load != NULL)
	{
		tileset_to_load->name = map_file_tilesetnode.attribute("name").as_string();
		tileset_to_load->firstgid = map_file_tilesetnode.attribute("firstgid").as_int();
		tileset_to_load->margin = map_file_tilesetnode.attribute("margin").as_int();
		tileset_to_load->spacing = map_file_tilesetnode.attribute("spacing").as_int();
		tileset_to_load->tile_width = map_file_tilesetnode.attribute("tile_width").as_int();
		tileset_to_load->tile_height = map_file_tilesetnode.attribute("tile_height").as_int();
		tileset_to_load->texture = App->tex->Load(map_file_tilesetnode.child("image").attribute("source").as_string());

		LOG("Tileset loaded correctly.");
		return(true);
	}
	else
	{
		LOG("Tileset could not be loaded.");
		return(false);
	}
}

// Load layer's data -------------------------------------------------------------

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
			
			map_node.tilesets.add(tileset_to_load);
		}
	}
	
	/*
	if (ret == true)
	{
		// loading the layers
		ret = LoadLayerData();
	}
	*/

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
	}

	map_loaded = ret;

	return ret;
}

