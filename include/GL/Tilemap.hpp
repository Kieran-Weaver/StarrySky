#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include <array>
#include <visit_struct/visit_struct.hpp>
using Tile = uint16_t;
enum TMType{
	Normal,
	Effect
};
struct TileMap{
	// UBO Data
	std::array<float,4> AffineT = {}; // Mat2 transformation of tilemap
	std::array<float,4> Attrs = {}; // Tile w, h, tilemap x, y
	std::array<float,4> metadata = {}; // Layer/Z, 3x Unused
	std::array<uint32_t,4> texData = {}; // Tile texture w, h, 2x unused
	uint32_t tileBufferTBO = 0; // Max 2^16 - 1 tile types
	uint32_t tileTextureTBO = 0; // Texture 
	// Internal Data
	std::vector<std::array<float, 4>> tileData = {};
	std::vector<Tile> drawn = {};
	uint32_t tileBuffer = 0; // Buffer for tileBufferTBO
	uint32_t tileTexture = 0;// Buffer for tileTextureTBO
	int numTiles = 0;
	std::vector<std::string> filenames = {};
	TMType type = TMType::Normal;
	bool initialized = false;
};
VISITABLE_STRUCT(TileMap, AffineT, Attrs, metadata, texData, filenames, drawn, type);
#endif
