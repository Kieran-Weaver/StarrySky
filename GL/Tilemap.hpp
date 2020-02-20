#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include <array>
using Tile = std::array<uint32_t, 2>; // 16 bits X-pos, 16 bits Y-pos, 24 bits data, 8 bits index
enum TMType{
	Normal,
	Effect
};
inline Tile makeTile(uint16_t x, uint16_t y, uint8_t index){
	return {static_cast<uint32_t>(x) | (static_cast<uint32_t>(y) << 16),  index};
}
inline void unpackTile(const Tile& tile, uint16_t& x, uint16_t& y, uint8_t& index){
	index = tile[1] & 0xFF;
	x = tile[0] & 0xFFFF;
	y = tile[0] >> 16;
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
