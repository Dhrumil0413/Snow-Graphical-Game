//
//  BlockMap.cpp
//

#include "BlockMap.h"

#include <cassert>
#include <string>
#include <iostream>
#include <fstream>

#include "ObjLibrary/Vector3.h"

using namespace std;
using namespace ObjLibrary;



BlockMap :: BlockMap ()
	: size_x(MAP_SIZE_MIN),
	  size_z(MAP_SIZE_MIN),
	  player_x(0),
	  player_z(0),
	  opponent_x(0),
	  opponent_z(0)
{
	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			stacks[x][z] = 0;

	assert(isInvariantTrue());
}

BlockMap :: BlockMap (int size_x_in, int size_z_in)
	: size_x(size_x_in),
	  size_z(size_z_in),
	  player_x(0),
	  player_z(0),
	  opponent_x(0),
	  opponent_z(0)
{
	assert(size_x_in >= MAP_SIZE_MIN);
	assert(size_z_in >= MAP_SIZE_MIN);
	assert(size_x_in <= MAP_SIZE_MAX);
	assert(size_z_in <= MAP_SIZE_MAX);

	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			stacks[x][z] = 0;

	assert(isInvariantTrue());
}

BlockMap :: BlockMap (const string& filename)
	: size_x(MAP_SIZE_MIN),
	  size_z(MAP_SIZE_MIN),
	  player_x(0),
	  player_z(0),
	  opponent_x(0),
	  opponent_z(0)
{
	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			stacks[x][z] = 0;
	// default initialization complete
	//  -> if we can't load the file, our object will still be valid

	ifstream fin(filename);
	if(!fin)
	{
		cerr << "Error: Could not open file \"" << filename << "\" for reading" << endl;
		assert(isInvariantTrue());
		return;  // stop immediately if we cannot open the data file
	}

	fin >> size_x;
	fin >> size_z;

	fin >> player_x;
	fin >> player_z;

	fin >> opponent_x;
	fin >> opponent_z;

	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			fin >> stacks[x][z];

	if(!fin)
		cerr << "Error: Something went wrong loading from file \"" << filename << "\"" << endl;

	assert(isInvariantTrue());
}



int BlockMap :: getSizeX () const
{
	return size_x;
}

int BlockMap :: getSizeZ () const
{
	return size_z;
}

bool BlockMap :: isInMap (int x, int z) const
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

unsigned int BlockMap :: getAt (int x, int z) const
{
	assert(isInMap(x, z));

	return stacks[x][z];
}

int BlockMap :: getPlayerX () const
{
	return player_x;
}

int BlockMap :: getPlayerZ () const
{
	return player_z;
}

Vector3 BlockMap :: getPlayerStart (double player_half_height) const
{
	assert(isInMap(player_x, player_z));
	double player_y = getAt(player_x, player_z) + player_half_height;
	return Vector3(player_x + 0.5, player_y, player_z + 0.5);
}

int BlockMap :: getOpponentX () const
{
	return opponent_x;
}

int BlockMap :: getOpponentZ () const
{
	return opponent_z;
}

Vector3 BlockMap :: getOpponentStart (double opponent_half_height) const
{
	assert(isInMap(opponent_x, opponent_z));
	double opponent_y = getAt(opponent_x, opponent_z) + opponent_half_height;
	return Vector3(opponent_x + 0.5, opponent_y, opponent_z + 0.5);
}

void BlockMap :: save (const string& filename) const
{
	ofstream fout(filename);
	if(!fout)
	{
		cerr << "Error: Could not open file \"" << filename << "\" for writing" << endl;
		assert(isInvariantTrue());
		return;  // stop immediately if we cannot open the data file
	}

	fout << size_x << " ";
	fout << size_z << endl;

	fout << player_x << " ";
	fout << player_z << endl;

	fout << opponent_x << " ";
	fout << opponent_z << endl;

	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
		{
			fout << stacks[x][z];
			if(z + 1 == size_z)
				fout << endl;
			else
				fout << " ";
		}

	if(!fout)
		cerr << "Error: Something went wrong saving to file \"" << filename << "\"" << endl;

	assert(isInvariantTrue());
}



void BlockMap :: setAt (int x, int z, unsigned int value)
{
	assert(isInMap(x, z));
	assert(value <= STACK_SIZE_MAX);

	stacks[x][z] = value;

	assert(isInvariantTrue());
}

void BlockMap :: setPlayerStart (int x, int z)
{
	assert(isInMap(x, z));

	player_x = x;
	player_z = z;

	assert(isInvariantTrue());
}

void BlockMap :: setOpponentStart (int x, int z)
{
	assert(isInMap(x, z));

	opponent_x = x;
	opponent_z = z;

	assert(isInvariantTrue());
}



bool BlockMap :: isInvariantTrue () const
{
	if(size_x < MAP_SIZE_MIN)
		return false;
	if(size_x > MAP_SIZE_MAX)
		return false;
	if(size_z < MAP_SIZE_MIN)
		return false;
	if(size_z > MAP_SIZE_MAX)
		return false;

	if(player_x <  0)
		return false;
	if(player_x >= size_x)
		return false;
	if(player_z <  0)
		return false;
	if(player_z >= size_z)
		return false;

	if(opponent_x <  0)
		return false;
	if(opponent_x >= size_x)
		return false;
	if(opponent_z <  0)
		return false;
	if(opponent_z >= size_z)
		return false;

	for(int x = 0; x < size_x; x++)
		for(int z = 0; z < size_z; z++)
			if(stacks[x][z] > STACK_SIZE_MAX)
				return false;

	return true;
}
