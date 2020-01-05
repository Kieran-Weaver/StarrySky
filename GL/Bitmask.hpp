#ifndef BITMASK_H
#define BITMASK_H
#include <memory>
#include <boost/dynamic_bitset.hpp>
struct Bitmask{
	uint16_t width=0;
	uint16_t height=0;
	std::shared_ptr<boost::dynamic_bitset<>> mask;
};

#endif
