#ifndef WRANGLE_HELPERS_H_
#define WRANGLE_HELPERS_H_

// like fmod, but returns a number in [-d/2,d/2]
float fmodp(float n, float d);

// constrain x to [lb, ub]
template <class T>
T clamp(T lb, T x, T ub);

// generate a random number in a range
template <class T>
T randm(T max);

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

#endif
