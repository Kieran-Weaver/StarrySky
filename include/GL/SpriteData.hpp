#ifndef STARRYSKY_SPRITEDATA_H
#define STARRYSKY_SPRITEDATA_H
#include <array>
#include <cstdint>
struct SpriteData{
	std::array<uint16_t,4> texRect = {};
	std::array<float,4> packedMat = {};
	std::array<float,4> sprPos = { 0.f, 0.f, 0.f, 1.f };
	std::array<uint8_t,4> sprColor = { 255, 255, 255, 255 };
};
#endif
