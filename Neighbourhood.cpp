//
//  Neighbourhood.cpp
//

#include"Neighbourhood.h"


#include"BlockMap.h"



Neighbourhood :: Neighbourhood ()
{
	fillZeros();
}

Neighbourhood :: Neighbourhood (int xmzm, int x0zm, int xpzm,
                                int xmz0,           int xpz0,
                                int xmzp, int x0zp, int xpzp)
{
	setAt(-1, -1, xmzm);
	setAt( 0, -1, x0zm);
	setAt( 1, -1, xpzm);

	setAt(-1,  0, xmz0);
	setAt( 0,  0, 0);
	setAt( 1,  0, xpz0);

	setAt(-1,  1, xmzp);
	setAt( 0,  1, x0zp);
	setAt( 1,  1, xpzp);
}

Neighbourhood :: Neighbourhood (const BlockMap& block_map,
                                int center_x, int center_z)
{
	if(block_map.isInMap(center_x, center_z))
	{
		int center_height = block_map.getAt(center_x, center_z);

		for(int dx = -1; dx <= 1; dx++)
			for(int dz = -1; dz <= 1; dz++)
			{
				int x = center_x + dx;
				int z = center_z + dz;
				if(block_map.isInMap(x, z))
				{
					int neighbour_height = block_map.getAt(x, z);
					if(neighbour_height < center_height)
						setAt(dx, dz, -1);
					else if(neighbour_height > center_height)
						setAt(dx, dz, 1);
					else
						setAt(dx, dz, 0);
				}
				else
				{
					// neighbour is outside block map
					setAt(dx, dz, -1);
				}
			}
	}
	else
	{
		// center is outside block map
		fillZeros();
	}
}



int Neighbourhood :: getAt (int dx, int dz) const
{
	assert(dx >= -1);
	assert(dx <=  1);
	assert(dz >= -1);
	assert(dz <=  1);

	return heights[dx + 1][dz + 1];
}

float Neighbourhood :: getEdgeDistance (float x_frac, float z_frac,
                                         int desired_value) const
{
	assert(x_frac >= 0.0f);
	assert(x_frac <= 1.0f);
	assert(z_frac >= 0.0f);
	assert(z_frac <= 1.0f);

	float best = 1.0f;

	// check edges
	float x0 = x_frac;
	float x1 = 1.0f - x_frac;
	float z0 = z_frac;
	float z1 = 1.0f - z_frac;
	if(getAt(-1,  0) == desired_value && best > x0)
		best = x0;
	if(getAt( 1,  0) == desired_value && best > x1)
		best = x1;
	if(getAt( 0, -1) == desired_value && best > z0)
		best = z0;
	if(getAt( 0,  1) == desired_value && best > z1)
		best = z1;

	// check corners
	float x0z0 = sqrt(x0 * x0 + z0 * z0);
	float x0z1 = sqrt(x0 * x0 + z1 * z1);
	float x1z0 = sqrt(x1 * x1 + z0 * z0);
	float x1z1 = sqrt(x1 * x1 + z1 * z1);
	if(getAt(-1, -1) == desired_value && best > x0z0)
		best = x0z0;
	if(getAt(-1,  1) == desired_value && best > x0z1)
		best = x0z1;
	if(getAt( 1, -1) == desired_value && best > x1z0)
		best = x1z0;
	if(getAt( 1,  1) == desired_value && best > x1z1)
		best = x1z1;

	// return best distance
	return best;
}



void Neighbourhood :: setAt (int dx, int dz, int value)
{
	assert(dx >= -1);
	assert(dx <=  1);
	assert(dz >= -1);
	assert(dz <=  1);
	assert(value >= -1);
	assert(value <=  1);

	heights[dx + 1][dz + 1] = value;
}



void Neighbourhood :: fillZeros ()
{
	for(int x = 0; x < 3; x++)
		for(int z = 0; z < 3; z++)
			heights[x][z] = 0;
}

