#include <cstdlib>
#include <cmath>
#include "helpers.hpp"

float fmodp(float n, float d)
{
	n = fmod(n, d);
	if(2 * n > d) n -= d;
	if(2 * n < -d) n += d;
	return n;
}

float rad2deg(float rad)
{
	return rad * 180.f / M_PI;
}

float deg2rad(float deg)
{
	return deg * M_PI / 180.f;
}

