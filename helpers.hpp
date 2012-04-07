#ifndef WRANGLE_HELPERS_H_
#define WRANGLE_HELPERS_H_

#include <cmath>
#include <SFML/System.hpp>

// like fmod, but returns a number in [-d/2,d/2]
float fmodp(float n, float d);

// constrain x to [lb, ub]
template <class T>
T clamp(T lb, T x, T ub);

// generate a random number in a range
template <class T>
T randm(T max);

// find distance between two points as sf::Vector2<T>
template <class T>
float v2dist(sf::Vector2<T>, sf::Vector2<T>);

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

template <class T>
float v2dist(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
	return sqrt((float)((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y)));
}

#endif
