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
