//
//  SnowMap.cpp
//

#include "SnowMap.h"

#include <cassert>

#include "GetGlut.h"

#include "ObjLibrary/DisplayList.h"



SnowMap :: SnowMap ()
		: size_x(MAP_SIZE_MIN),
		  size_z(MAP_SIZE_MIN)
{
	assert(isInvariantTrue());
}

SnowMap :: SnowMap (int size_x_in, int size_z_in)
		: size_x(size_x_in),
		  size_z(size_z_in)
{
	assert(isInvariantTrue());
}

int SnowMap :: getSizeX () const
{
	return size_x;
}

int SnowMap :: getSizeZ () const
{
	return size_z;
}

bool SnowMap :: isInMap (int x, int z) const
{
	if(x <  0)
		return false;
	if(x >= size_x)
		return false;
	if(z <  0)
		return false;
	if(z >= size_z)
		return false;
	return true;
}

bool SnowMap :: isSetAt (int x, int z) const
{
	assert(isInMap(x, z));

	return heightmaps[x][z].isReady();
}

bool SnowMap :: isSetAll () const
{
	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			if(!heightmaps[x][z].isReady())
				return false;
	return true;
}

void SnowMap :: drawAround (int x, int z, int radius) const
{
	assert(isSetAll());

	int min_x = x - radius;
	if(min_x < 0)
		min_x = 0;
	int max_x = x + radius;
	if(max_x > size_x - 1)
		max_x = size_x - 1;

	int min_z = z - radius;
	if(min_z < 0)
		min_z = 0;
	int max_z = z + radius;
	if(max_z > size_z - 1)
		max_z = size_z - 1;

	for(int x2 = min_x; x2 <= max_x; x2++)
		for(int z2 = min_z; z2 <= max_z; z2++)
		{
			assert(x2 >= 0);
			assert(x2 <  size_x);
			assert(z2 >= 0);
			assert(z2 <  size_z);
			assert(heightmaps[x2][z2].isReady());

			//glPushMatrix();
			//	glTranslated(x2, 0.0, z2);
				heightmaps[x2][z2].draw();
			//glPopMatrix();
		}
}



void SnowMap :: setAt (int x, int z, const ObjLibrary::DisplayList& heightmap)
{
	assert(isInMap(x, z));
	assert(heightmap.isReady());

	heightmaps[x][z] = heightmap;
}



bool SnowMap :: isInvariantTrue () const
{
	if(size_x < MAP_SIZE_MIN)
		return false;
	if(size_x > MAP_SIZE_MAX)
		return false;
	if(size_z < MAP_SIZE_MIN)
		return false;
	if(size_z > MAP_SIZE_MAX)
		return false;
	return true;
}

