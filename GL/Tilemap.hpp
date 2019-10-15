#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include "Helpers.hpp"
struct TileMap{
	float affineT[4]; // 16 bytes
	float tileSize[2];
	float tilepadding[2]; // 16 bytes
	float tiles[256][4]; // Max 255 tile types
};
#endif
