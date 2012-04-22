#ifndef WRANGLE_HELPERS_H_
#define WRANGLE_HELPERS_H_

#include <cmath>
#include <algorithm>
#include <SFML/System.hpp>

// like fmod, but returns a number in [-d/2,d/2]
float fmodp(float n, float d);

float rad2deg(float rad);
float deg2rad(float deg);

// constrain x to [lb, ub]
template <class T>
T clamp(T lb, T x, T ub)
{
	return (x < lb ? lb : (x > ub ? ub : x));
}

// generate a random number in a range
template <class T>
T randm(T max)
{
	return (T)(max * ((float)rand() / (float)RAND_MAX));
}

// find magnitde of sf::Vector2<T>
template <class T>
float v2mag(sf::Vector2<T> v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

// find the dot product of two vectors
template <class T>
float v2dot(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

// find the angle between two vectors
template <class T>
float v2angle(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
	return rad2deg(acos(clamp<double>(-1, v2dot(v1, v2) / (v2mag(v1) * v2mag(v2)), 1)));
}

// find distance between two points as sf::Vector2<T>
template <class T>
float v2dist(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
	return std::sqrt((float)((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y)));
}

// return a parallel vector with at most max magnitude
template <class T>
sf::Vector2<T> truncate(sf::Vector2<T> v, T max)
{
	T mag = v2mag(v);
	if (mag > max)
		return v * max / mag;
	else
		return v;
}

#endif
