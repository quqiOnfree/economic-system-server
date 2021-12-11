#pragma once
#ifndef Q_RANDOM_HPP
#define Q_RANDOM_HPP

#include <vector>

#define MAX_LONG_LONG 18446744073709551615

namespace q_random
{
	_declspec(dllexport) long long randint(long long mn, long long mx);
	_declspec(dllexport) long double random();
	_declspec(dllexport) long double uniform(long double mn, long double mx);
	_declspec(dllexport) std::vector<long long> randrange(long long size);
}

#endif // !Q_RANDOM_HPP
