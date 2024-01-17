//
//  PerlinNoiseField.h
//
//  A module to calculate value noise and Perlin noise.
//

#include <cassert>
#include <cmath>
#include <climits>

#include "ObjLibrary/Vector2.h"

#include "PerlinNoiseField.h"

using namespace ObjLibrary;
namespace
{
	const unsigned int DEFAULT_SEED_X1 = 1273472206;
	const unsigned int DEFAULT_SEED_X2 = 4278162623;
	const unsigned int DEFAULT_SEED_Y1 = 1440014778;
	const unsigned int DEFAULT_SEED_Y2 =  524485263;
	const unsigned int DEFAULT_SEED_Q0 = 1498573726;
	const unsigned int DEFAULT_SEED_Q1 = 3476519523;
	const unsigned int DEFAULT_SEED_Q2 = 3905844518;
}



PerlinNoiseField :: PerlinNoiseField ()
		: m_grid_size(1.0f)
		, m_amplitude(1.0f)
		, m_seed_x1(DEFAULT_SEED_X1)
		, m_seed_x2(DEFAULT_SEED_X2)
		, m_seed_y1(DEFAULT_SEED_Y1)
		, m_seed_y2(DEFAULT_SEED_Y2)
		, m_seed_q0(DEFAULT_SEED_Q0)
		, m_seed_q1(DEFAULT_SEED_Q1)
		, m_seed_q2(DEFAULT_SEED_Q2)
{
	assert(invariant());
}

PerlinNoiseField :: PerlinNoiseField (float grid_size,
                                      float amplitude)
		: m_grid_size(grid_size)
		, m_amplitude(amplitude)
		, m_seed_x1(DEFAULT_SEED_X1)
		, m_seed_x2(DEFAULT_SEED_X2)
		, m_seed_y1(DEFAULT_SEED_Y1)
		, m_seed_y2(DEFAULT_SEED_Y2)
		, m_seed_q0(DEFAULT_SEED_Q0)
		, m_seed_q1(DEFAULT_SEED_Q1)
		, m_seed_q2(DEFAULT_SEED_Q2)
{
	assert(grid_size > 0.0f);

	assert(invariant());
}

PerlinNoiseField :: PerlinNoiseField (float grid_size,
                                      float amplitude,
                                      unsigned int seed_x1,
                                      unsigned int seed_x2,
                                      unsigned int seed_y1,
                                      unsigned int seed_y2,
                                      unsigned int seed_q0,
                                      unsigned int seed_q1,
                                      unsigned int seed_q2)
		: m_grid_size(grid_size)
		, m_amplitude(amplitude)
		, m_seed_x1(seed_x1)
		, m_seed_x2(seed_x2)
		, m_seed_y1(seed_y1)
		, m_seed_y2(seed_y2)
		, m_seed_q0(seed_q0)
		, m_seed_q1(seed_q1)
		, m_seed_q2(seed_q2)
{
	assert(grid_size > 0.0f);

	assert(invariant());
}



float PerlinNoiseField :: getGridSize () const
{
	return m_grid_size;
}

float PerlinNoiseField :: getAmplitude () const
{
	return m_amplitude;
}

float PerlinNoiseField :: valueNoise (float x, float y) const
{
	int x0 = (int)(floor(x / m_grid_size));
	int y0 = (int)(floor(y / m_grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float x_frac = x / m_grid_size - x0;
	float y_frac = y / m_grid_size - y0;

	float x_fade = fade(x_frac);
	float y_fade = fade(y_frac);

	unsigned int value00 = pseudorandom(x0, y0);
	unsigned int value01 = pseudorandom(x0, y1);
	unsigned int value10 = pseudorandom(x1, y0);
	unsigned int value11 = pseudorandom(x1, y1);

	unsigned int value0 = interpolate(value00, value01, y_fade);
	unsigned int value1 = interpolate(value10, value11, y_fade);
	unsigned int value  = interpolate(value0,  value1,  x_fade);

	return unsignedIntToPM1(value) * m_amplitude;
}

float PerlinNoiseField :: perlinNoise (float x, float y) const
{
	int x0 = (int)(floor(x / m_grid_size));
	int y0 = (int)(floor(y / m_grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	float x_frac = x / m_grid_size - x0;
	float y_frac = y / m_grid_size - y0;

	float x_fade = fade(x_frac);
	float y_fade = fade(y_frac);

	Vector2 lattice00 = lattice(x0, y0);
	Vector2 lattice01 = lattice(x0, y1);
	Vector2 lattice10 = lattice(x1, y0);
	Vector2 lattice11 = lattice(x1, y1);

	Vector2 direction00(     - x_frac,      - y_frac);
	Vector2 direction01(     - x_frac, 1.0f - y_frac);
	Vector2 direction10(1.0f - x_frac,      - y_frac);
	Vector2 direction11(1.0f - x_frac, 1.0f - y_frac);

	float value00 = (float)(lattice00.dotProduct(direction00));
	float value01 = (float)(lattice01.dotProduct(direction01));
	float value10 = (float)(lattice10.dotProduct(direction10));
	float value11 = (float)(lattice11.dotProduct(direction11));

	float value0 = interpolate(value00, value01, y_fade);
	float value1 = interpolate(value10, value11, y_fade);
	float value  = interpolate(value0,  value1,  x_fade);

	return value * m_amplitude;
}



void PerlinNoiseField :: init (float grid_size,
                               float amplitude,
                               unsigned int seed_x1,
                               unsigned int seed_x2,
                               unsigned int seed_y1,
                               unsigned int seed_y2,
                               unsigned int seed_q0,
                               unsigned int seed_q1,
                               unsigned int seed_q2)
{
	assert(grid_size > 0.0f);

	m_grid_size = grid_size;
	m_amplitude = amplitude;

	m_seed_x1 = seed_x1;
	m_seed_x2 = seed_x2;
	m_seed_y1 = seed_y1;
	m_seed_y2 = seed_y2;
	m_seed_q0 = seed_q0;
	m_seed_q1 = seed_q1;
	m_seed_q2 = seed_q2;

	assert(invariant());
}

void PerlinNoiseField :: setGridSize (float grid_size)
{
	assert(grid_size > 0.0f);

	m_grid_size = grid_size;

	assert(invariant());
}

void PerlinNoiseField :: setAmplitude (float amplitude)
{
	m_amplitude = amplitude;

	assert(invariant());
}

void PerlinNoiseField :: setSeeds (unsigned int seed_x1,
                                   unsigned int seed_x2,
                                   unsigned int seed_y1,
                                   unsigned int seed_y2,
                                   unsigned int seed_q0,
                                   unsigned int seed_q1,
                                   unsigned int seed_q2)
{
	m_seed_x1 = seed_x1;
	m_seed_x2 = seed_x2;
	m_seed_y1 = seed_y1;
	m_seed_y2 = seed_y2;
	m_seed_q0 = seed_q0;
	m_seed_q1 = seed_q1;
	m_seed_q2 = seed_q2;

	assert(invariant());
}



unsigned int PerlinNoiseField :: pseudorandom (int x, int y) const
{
	unsigned int n = (m_seed_x1 * x) +
	                 (m_seed_y1 * y);
	unsigned int quad_term = m_seed_q2 * n * n +
	                         m_seed_q1 * n     +
	                         m_seed_q0;
	return quad_term +
	       (m_seed_x2 * x) +
	       (m_seed_y2 * y);
}

float PerlinNoiseField :: unsignedIntToPM1 (unsigned int n) const
{
	return ((float)(n) / UINT_MAX) * 2.0f - 1.0f;
}

float PerlinNoiseField :: fade (float n) const
{
	//return n;
	//return (-2.0f * n + 3) * n * n;
	//return ((6 * n - 15) * n + 10) * n * n * n;
	return (1 - cos(n * 3.14159265f)) * 0.5f;
}

unsigned int PerlinNoiseField :: interpolate (unsigned int v0,
                                              unsigned int v1,
                                              float fraction) const
{
	return (unsigned int)(v0 * (1.0f - fraction)) +
	       (unsigned int)(v1 *         fraction );
}

float PerlinNoiseField :: interpolate (float v0,
                                       float v1,
                                       float fraction) const
{
	return (v0 * (1.0f - fraction)) +
	       (v1 *         fraction );
}

ObjLibrary::Vector2 PerlinNoiseField :: lattice (int x, int y) const
{
	unsigned int value = pseudorandom(x, y);
	float radians = (float)(value);  // very random
	return Vector2(cos(radians), sin(radians));
}

bool PerlinNoiseField :: invariant () const
{
	if(m_grid_size <= 0.0) return false;
	return true;
}
