//
//  PerlinNoiseField.h
//
//  A module to calculate value noise and Perlin noise.
//

#pragma once

#include "ObjLibrary/Vector2.h"



//
//  PerlinNoiseField.h
//
//  A class to calculate value noise and Perlin noise.
//
//  Class Invariant:
//    <1> m_grid_size > 0.0
//
class PerlinNoiseField
{
public:
	PerlinNoiseField ();
	PerlinNoiseField (float grid_size,
	                  float amplitude);
	PerlinNoiseField (float grid_size,
	                  float amplitude,
	                  unsigned int seed_x1,
	                  unsigned int seed_x2,
	                  unsigned int seed_y1,
	                  unsigned int seed_y2,
	                  unsigned int seed_q0,
	                  unsigned int seed_q1,
	                  unsigned int seed_q2);
	PerlinNoiseField (const PerlinNoiseField& to_copy) = default;
	~PerlinNoiseField () = default;
	PerlinNoiseField& operator= (const PerlinNoiseField& to_copy) = default;

	float getGridSize () const;
	float getAmplitude () const;
	float valueNoise (float x, float y) const;
	float perlinNoise (float x, float y) const;
	void init (float grid_size,
	           float amplitude,
	           unsigned int seed_x1,
	           unsigned int seed_x2,
	           unsigned int seed_y1,
	           unsigned int seed_y2,
	           unsigned int seed_q0,
	           unsigned int seed_q1,
	           unsigned int seed_q2);
	void setGridSize (float grid_size);
	void setAmplitude (float amplitude);
	void setSeeds (unsigned int seed_x1,
	               unsigned int seed_x2,
	               unsigned int seed_y1,
	               unsigned int seed_y2,
	               unsigned int seed_q0,
	               unsigned int seed_q1,
	               unsigned int seed_q2);

private:
	unsigned int pseudorandom (int x, int y) const;
	float unsignedIntToPM1 (unsigned int n) const;
	float fade (float n) const;
	unsigned int interpolate (unsigned int v0,
	                          unsigned int v1,
	                          float fraction) const;
	float interpolate (float v0,
	                   float v1,
	                   float fraction) const;
	ObjLibrary::Vector2 lattice (int x, int y) const;
	bool invariant () const;

private:
	float m_grid_size;
	float m_amplitude;
	unsigned int m_seed_x1;
	unsigned int m_seed_x2;
	unsigned int m_seed_y1;
	unsigned int m_seed_y2;
	unsigned int m_seed_q0;
	unsigned int m_seed_q1;
	unsigned int m_seed_q2;
};
