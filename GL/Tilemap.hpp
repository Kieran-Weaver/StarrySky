#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include "Helpers.hpp"
using Tile = uint32_t; // 12 bits X-pos, 12 bits Y-pos, 8 bits index
enum TMState{
	Normal,
	Effect
};
struct TileMap{
	// UBO Data
	float affineT[4]; // 16 bytes
	float packedtileSize[4]; // 16 bytes
	float offset[4];
	float tiles[256][4]; // Max 255 tile types
	// Internal Data
	int numTiles;
	std::vector<std::string> filenames;
	std::vector<Tile> drawn;
	TMState state = TMState::Normal;
};
#endif
