//
//  Heightmap.h
//

#pragma once

#include <string>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"


const int HEIGHTMAP_VERTEX_SIZE = 9;


class Heightmap
{
public:
	Heightmap ();
	Heightmap (float texture_repeat1, float texture_offset1);
	float getTextureRepeat () const;
	float getTextureOffset () const;
	float getAt (int x, int z) const;
	bool isDisplayList () const;
	const ObjLibrary::DisplayList& getDisplayList () const;
	void draw () const;

	void setAt (int x, int z, float value);
	void initDisplayList (const ObjLibrary::Vector3& offset,
	                      const std::string& texture);

private:
	float heights[HEIGHTMAP_VERTEX_SIZE][HEIGHTMAP_VERTEX_SIZE];
	float texture_repeat;
	float texture_offset;
	ObjLibrary::DisplayList display_list;
};

