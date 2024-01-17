//
//  Heightmap.cpp
//

#include "Heightmap.h"

#include <cassert>
#include <cstdlib>

#include "GetGlut.h"

#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/TextureManager.h"

using namespace ObjLibrary;



Heightmap :: Heightmap ()
		: texture_repeat(1.0),
		  texture_offset(0.0)
{
	for(unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
		for(unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
			heights[x][z] = 0.0f;
}



Heightmap :: Heightmap (float texture_repeat1, float texture_offset1)
		: texture_repeat(texture_repeat1),
		  texture_offset(texture_offset1)
{
	for(unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
		for(unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
			heights[x][z] = 0.0f;
}



float Heightmap :: getTextureRepeat () const
{
	return texture_repeat;
}

float Heightmap :: getTextureOffset () const
{
	return texture_offset;
}

float Heightmap :: getAt (int x, int z) const
{
	assert(x >= 0);
	assert(x <  HEIGHTMAP_VERTEX_SIZE);
	assert(z >= 0);
	assert(z <  HEIGHTMAP_VERTEX_SIZE);

	return heights[x][z];
}

bool Heightmap :: isDisplayList () const
{
	return display_list.isReady();
}

const ObjLibrary::DisplayList& Heightmap :: getDisplayList () const
{
	return display_list;
}

void Heightmap :: draw () const
{
	assert(isDisplayList());

	display_list.draw();
}



void Heightmap :: setAt (int x, int z, float value)
{
	assert(x >= 0);
	assert(x <  HEIGHTMAP_VERTEX_SIZE);
	assert(z >= 0);
	assert(z <  HEIGHTMAP_VERTEX_SIZE);

	heights[x][z] = value;
}

void Heightmap :: initDisplayList (const ObjLibrary::Vector3& offset,
                                   const std::string& texture)
{
	TextureManager::activate(texture);

	display_list.begin();
		glEnable(GL_TEXTURE_2D);
		TextureManager::activate(texture);
		glColor3d(1.0, 1.0, 1.0);

		for(unsigned int x1 = 1; x1 < HEIGHTMAP_VERTEX_SIZE; x1++)
		{
			unsigned int x0 = x1 - 1;
			double x0_frac = x0 / (HEIGHTMAP_VERTEX_SIZE - 1.0);
			double x1_frac = x1 / (HEIGHTMAP_VERTEX_SIZE - 1.0);
			double x0_vert = offset.x + x0_frac;
			double x1_vert = offset.x + x1_frac;
			double x0_tex  = texture_offset + texture_repeat * x0_frac;
			double x1_tex  = texture_offset + texture_repeat * x1_frac;

			glBegin(GL_TRIANGLE_STRIP);
				for(unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
				{
					double z_frac = z / (HEIGHTMAP_VERTEX_SIZE - 1.0);
					double z_vert = offset.z + z_frac;
					double z_tex  = texture_offset + texture_repeat * z_frac;

					double y0_vert = offset.y + heights[x0][z];
					double y1_vert = offset.y + heights[x1][z];

					glTexCoord2d(x1_tex, z_tex);
					glVertex3d(x1_vert, y1_vert, z_vert);
					glTexCoord2d(x0_tex, z_tex);
					glVertex3d(x0_vert, y0_vert, z_vert);
				}
			glEnd();
		}

		glDisable(GL_TEXTURE_2D);
	display_list.end();

	assert(isDisplayList());
}

