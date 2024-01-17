//
//  Map.h
//

#pragma once

#include <string>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "PerlinNoiseField.h"
#include "Heightmap.h"
#include "BlockMap.h"
#include "SnowMap.h"
#include "Neighbourhood.h"



class Map
{
public:
	static const unsigned int MAP_SIZE_MIN = BlockMap::MAP_SIZE_MIN;
	static const unsigned int MAP_SIZE_MAX = BlockMap::MAP_SIZE_MAX;

public:
	Map ();
	Map (int size_x_in, int size_z_in,
	     const ObjLibrary::DisplayList& block_display_list,
	     const std::string& snow_texture_filename);
	Map (const std::string& map_filename,
	     const ObjLibrary::DisplayList& block_display_list,
	     const std::string& snow_texture_filename);

	bool isInitialized () const;

	int getSizeX () const;
	int getSizeZ () const;
	bool isInMap (int x, int z) const;

	const BlockMap& getBlockMap () const;
	ObjLibrary::Vector3 getPlayerStart (double player_half_height) const;
	ObjLibrary::Vector3 getOpponentStart (double player_half_height) const;
	unsigned int getBlockHeight (int x, int z) const;
	float getSnowHeightAtPoint(int base_x, int base_z, float x_frac, float z_frac) const;

	Neighbourhood getNeighbourhood (int center_x, int center_z) const;

	const PerlinNoiseField& getNoiseLarge () const;
	const PerlinNoiseField& getNoiseSmall () const;
	const SnowMap& getSnowMap () const;

	void drawBlocks () const;
	void drawSnow (int center_x, int center_z,
	               int snow_radius) const;

private:
	void initSnowMap (const std::string& texture_filename);
	Heightmap initSnowHeightmap (int base_x, int base_z);
	float calculateSnowHeight (int base_x, int base_z,
	                           const Neighbourhood& neighbourhood,
	                           float x_frac, float z_frac) const;

	void drawBlock (int x, int y, int z) const;

private:
	BlockMap block_map;
	ObjLibrary::DisplayList block_list;

	PerlinNoiseField noise_large;
	PerlinNoiseField noise_small;
	SnowMap snow_map;
};
