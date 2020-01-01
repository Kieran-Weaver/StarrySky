#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
using Tile = uint32_t; // 12 bits X-pos, 12 bits Y-pos, 8 bits index
enum TMState{
	Normal,
	Effect
};
inline Tile makeTile(uint16_t x, uint16_t y, uint8_t index){
	return ((x & 0xFFF) << 20) | ((y & 0xFFF) << 8) | index;
}
struct TileMap{
	// UBO Data
	float affineT[4]; // 16 bytes
	float packedtileSize[4]; // 16 bytes
	float tiles[256][4]; // Max 255 tile types
	// Internal Data
	int numTiles;
	std::vector<std::string> filenames;
	std::vector<Tile> drawn;
	TMState state = TMState::Normal;
};
#endif
