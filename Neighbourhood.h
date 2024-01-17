//
//  Neighbourhood.h
//

#pragma once

#include "Heightmap.h"



class BlockMap;

class Neighbourhood
{
public:
	Neighbourhood ();
	Neighbourhood (int xmzm, int x0zm, int xpzm,
	               int xmz0,           int xpz0,
	               int xmzp, int x0zp, int xpzp);
	Neighbourhood (const BlockMap& block_map,
	               int center_x, int center_z);

	int getAt (int dx, int dz) const;
	float getEdgeDistance (float x_frac, float z_frac,
	                       int desired_value) const;

	void setAt (int dx, int dz, int value);

private:
	void fillZeros ();

private:
	int heights[3][3];
};
