//
//  SnowMap.h
//

#pragma once

#include "ObjLibrary/DisplayList.h"

class SnowMap
{
public:
	static const unsigned int MAP_SIZE_MIN =   1;
	static const unsigned int MAP_SIZE_MAX = 100;

public:
	SnowMap ();
	SnowMap (int size_x_in, int size_z_in);

	int getSizeX () const;
	int getSizeZ () const;
	bool isInMap (int x, int z) const;
	bool isSetAt (int x, int z) const;
	bool isSetAll () const;

	void drawAround (int x, int z, int radius) const;

	void setAt (int x, int z, const ObjLibrary::DisplayList& heightmap);

private:
	bool isInvariantTrue () const;

private:
	int size_x;
	int size_z;
	ObjLibrary::DisplayList heightmaps[MAP_SIZE_MAX][MAP_SIZE_MAX];
};
