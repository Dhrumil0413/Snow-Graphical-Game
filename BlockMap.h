//
//  BlockMap.h
//

#pragma once

#include <string>

#include "ObjLibrary/Vector3.h"



class BlockMap
{
public:
	static const unsigned int MAP_SIZE_MIN =   1;
	static const unsigned int MAP_SIZE_MAX = 100;

	static const unsigned int STACK_SIZE_MAX = 9;

public:
	BlockMap ();
	BlockMap (int size_x_in, int size_z_in);
	BlockMap (const std::string& filename);

	int getSizeX () const;
	int getSizeZ () const;
	bool isInMap (int x, int z) const;
	unsigned int getAt (int x, int z) const;

	int getPlayerX () const;
	int getPlayerZ () const;
	ObjLibrary::Vector3 getPlayerStart (double player_half_height) const;

	int getOpponentX () const;
	int getOpponentZ () const;
	ObjLibrary::Vector3 getOpponentStart (double opponent_half_height) const;

	void save (const std::string& filename) const;

	void setAt (int x, int z, unsigned int value);
	void setPlayerStart (int x, int z);
	void setOpponentStart (int x, int z);

private:
	bool isInvariantTrue () const;

private:
	int size_x;
	int size_z;
	int player_x;
	int player_z;
	int opponent_x;
	int opponent_z;
	unsigned int stacks[MAP_SIZE_MAX][MAP_SIZE_MAX];
};
