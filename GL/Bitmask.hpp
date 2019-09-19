#ifndef BITMASK_H
#define BITMASK_H

#include <boost/dynamic_bitset.hpp>

struct Bitmask{
	uint16_t width=0;
	uint16_t height=0;
	boost::dynamic_bitset<> mask;
};

#endif