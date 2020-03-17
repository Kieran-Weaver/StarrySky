#ifndef BITMASK_H
#define BITMASK_H
#include <memory>
struct Bitmask{
	uint16_t width=0;
	uint16_t height=0;
	std::shared_ptr<uint32_t> mask;
	bool get(uint16_t x, uint16_t y) const{
		size_t index = static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x);
		const size_t offset = index % 32;
		const size_t omask = 1 << offset;
		uint32_t data = mask.get()[index/32];
		return (data & omask) >> offset;
	}
};

#endif
