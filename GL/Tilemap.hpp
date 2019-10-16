#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include "Helpers.hpp"
struct Tile{
	float px;
	float py;
	uint16_t index;
	uint16_t ignored;
};
struct TileMap{
	// UBO Data
	float affineT[4]; // 16 bytes
	float packedtileSize[4]; // 16 bytes
	float tiles[256][4]; // Max 255 tile types
	// Internal Data
	int numTiles;
	std::vector<std::string> filenames;
	std::vector<Tile> drawn;
};
#endif
