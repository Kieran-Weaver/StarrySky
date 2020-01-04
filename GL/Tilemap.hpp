#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include <array>
using Tile = uint32_t; // 12 bits X-pos, 12 bits Y-pos, 8 bits index
enum TMType{
	Normal,
	Effect
};
inline Tile makeTile(uint16_t x, uint16_t y, uint8_t index){
	return ((x & 0xFFF) << 20) | ((y & 0xFFF) << 8) | index;
}
struct TileMap{
	// UBO Data
	std::array<float,4> affineT; // 16 bytes
	std::array<float,4> packedtileSize; // 16 bytes
	std::array<std::array<float,4>,256> tiles; // Max 255 tile types
	// Internal Data
	int numTiles;
	std::vector<std::string> filenames;
	std::vector<Tile> drawn;
	TMType type = TMType::Normal;
};
#endif
