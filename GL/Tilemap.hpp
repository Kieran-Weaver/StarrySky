#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <vector>
#include <cstdint>
struct Sprite;
struct TileMap{
	Sprite * palette = nullptr;
	std::vector< std::vector<uint8_t> > tiles; // Max 255 tile types
	const uint16_t tilex, tiley;
};
#endif