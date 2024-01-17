//
//  Angles.h
//

#pragma once



const double PI      = 3.1415926535897932384626433832795;
const double TWO_PI  = PI * 2.0;
const double HALF_PI = PI * 0.5;

inline double degreesToRadians (double degrees)
{
	return degrees * PI / 180.0;
}

inline double radiansToDegrees (double radians)
{
	return radians * 180.0 / PI;
}
