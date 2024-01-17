//
//  Map.cpp
//

#include "Map.h"

#include <cassert>
#include <string>

#include "GetGlut.h"

#include "ObjLibrary/DisplayList.h"

#include "BlockMap.h"
#include "SnowMap.h"

using namespace std;
using namespace ObjLibrary;



Map :: Map ()
		: block_map(),
		  block_list(),
		  snow_map()
{
	assert(!isInitialized());
}

Map :: Map (int size_x_in, int size_z_in,
            const DisplayList& block_display_list,
            const string& snow_texture_filename)
		: block_map(size_x_in, size_z_in),
		  block_list(block_display_list)
		  // snow_map is initialized below
{
	initSnowMap(snow_texture_filename);

	assert(isInitialized());
}

Map :: Map (const string& map_filename,
            const DisplayList& block_display_list,
            const string& snow_texture_filename)
		: block_map(map_filename),
		  block_list(block_display_list)
		  // snow_map is initialized below
{
	initSnowMap(snow_texture_filename);

	assert(isInitialized());
}



bool Map :: isInitialized () const
{
	return snow_map.isSetAll();
}

int Map :: getSizeX () const
{
	assert(isInitialized());

	assert(block_map.getSizeX() == snow_map.getSizeX());
	return block_map.getSizeX();
}

int Map :: getSizeZ () const
{
	assert(isInitialized());

	assert(block_map.getSizeZ() == snow_map.getSizeZ());
	return block_map.getSizeZ();
}

bool Map :: isInMap (int x, int z) const
{
	assert(isInitialized());

	assert(block_map.isInMap(x, z) == snow_map.isInMap(x, z));
	return block_map.isInMap(x, z);
}



const BlockMap& Map :: getBlockMap () const
{
	assert(isInitialized());

	return block_map;
}

Vector3 Map :: getPlayerStart (double player_half_height) const
{
	assert(isInitialized());

	return block_map.getPlayerStart(player_half_height);
}

Vector3 Map :: getOpponentStart (double player_half_height) const
{
	assert(isInitialized());

	return block_map.getOpponentStart(player_half_height);
}

unsigned int Map :: getBlockHeight (int x, int z) const
{
	assert(isInitialized());

	if(isInMap(x, z))
		return block_map.getAt(x, z);
	else
		return 0;
}

float Map::getSnowHeightAtPoint(int base_x, int base_z, float x_frac, float z_frac) const
{
	Neighbourhood neighbourhood(block_map, base_x, base_z);

	return calculateSnowHeight(base_x, base_z, neighbourhood, x_frac, z_frac);
}


Neighbourhood Map :: getNeighbourhood (int center_x, int center_z) const
{
	assert(isInitialized());

	return Neighbourhood(block_map, center_x, center_z);
}

const PerlinNoiseField& Map :: getNoiseLarge () const
{
	assert(isInitialized());

	return noise_large;
}

const PerlinNoiseField& Map :: getNoiseSmall () const
{
	assert(isInitialized());

	return noise_small;
}

const SnowMap& Map :: getSnowMap () const
{
	assert(isInitialized());

	return snow_map;
}



void Map :: drawBlocks () const
{
	assert(isInitialized());

	for(int x = 0; x < block_map.getSizeX(); x++)
		for(int z = 0; z < block_map.getSizeZ(); z++)
		{
			unsigned int count = block_map.getAt(x, z);
			for(unsigned int y = 0; y < count; y++)
				drawBlock(x, y, z);
		}
}

void Map :: drawSnow (int center_x, int center_z,
                      int snow_radius) const
{
	assert(isInitialized());
	assert(snow_radius >= 0);

	snow_map.drawAround(center_x, center_z, snow_radius);
}



void Map :: initSnowMap (const string& texture_filename)
{
	noise_large = PerlinNoiseField(2.8f, 0.6f,
	                               0xf574c27b, 0x0ed3ee2f, 0x38638a3b, 0x875f9dc9,
	                               0x5bc9cb6e, 0x729a45e1, 0xe0d7f47e);
	noise_small = PerlinNoiseField(0.43f, 0.3f,
	                               0xc11df3f8, 0xc9c2b70e, 0xa5f71ab8, 0xed536273,
	                               0xd539c909, 0x777cdf43, 0x23da6048);

	snow_map = SnowMap(block_map.getSizeX(), block_map.getSizeZ());

	for(int x = 0; x < snow_map.getSizeX(); x++)
		for(int z = 0; z < snow_map.getSizeZ(); z++)
		{
			assert(block_map.isInMap(x, z));
			Vector3 offset = Vector3(x, block_map.getAt(x, z), z);

			Heightmap heightmap = initSnowHeightmap(x, z);
			heightmap.initDisplayList(offset, texture_filename);
			snow_map.setAt(x, z, heightmap.getDisplayList());
		}
}

Heightmap Map :: initSnowHeightmap (int base_x, int base_z)
{
	Neighbourhood neighbourhood(block_map, base_x, base_z);

	Heightmap snow;
	for(int vx = 0; vx < HEIGHTMAP_VERTEX_SIZE; vx++)
		for(int vz = 0; vz < HEIGHTMAP_VERTEX_SIZE; vz++)
		{
			float x_frac = vx / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float z_frac = vz / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float value = calculateSnowHeight(base_x, base_z, neighbourhood, x_frac, z_frac);
			snow.setAt(vx, vz, value);
		}
	return snow;
}

float Map :: calculateSnowHeight (int base_x, int base_z,
                                  const Neighbourhood& neighbourhood,
                                  float x_frac, float z_frac) const
{
	float value_large  = noise_large.perlinNoise(base_x + x_frac, base_z + z_frac);
	float value_small  = noise_small.perlinNoise(base_x + x_frac, base_z + z_frac);
	float value_higher = (float)(neighbourhood.getEdgeDistance(x_frac, z_frac,  1));
	float value_lower  = (float)(neighbourhood.getEdgeDistance(x_frac, z_frac, -1));

	float value_noise = value_large + value_small;
	float value_edges = sqrt(value_lower) * 0.4f - sqrt(value_higher) * 0.4f;
	//float value_edges = value_lower * 0.3f - value_higher * 0.3f;
	float value_total = value_noise + value_edges;
	if(value_total > value_lower)
		value_total = value_lower;

	return value_total;
}



void Map :: drawBlock (int x, int y, int z) const
{
	glPushMatrix();
		glTranslated(x + 0.5, y + 0.5, z + 0.5);
		block_list.draw();
	glPopMatrix();
}

