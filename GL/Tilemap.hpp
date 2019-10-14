#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
struct TileMap{
	float affineT[4];
	int numTiles = 0;
	int padding[3];
	float tiles[4][256]; // Max 255 tile types
};
#endif
