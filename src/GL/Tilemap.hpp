#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include <array>
using Tile = uint16_t;
enum TMType{
	Normal,
	Effect
};
struct TileMap{
	// UBO Data
	std::array<float,4> affineT; // Mat2 transformation of tilemap
	std::array<float,4> packedtileSize; // Tile w, h, tilemap x, y
	std::array<float,4> metadata; // Layer/Z, 3x Unused
	std::array<uint32_t,4> texdata; // Tile texture w, h, 2x unused
	GLuint tileBufferTBO; // Max 2^16 - 1 tile types
	GLuint tileTextureTBO; // Texture 
	// Internal Data
	std::vector<std::array<float, 4>> tileData;
	std::vector<Tile> drawn;
	GLuint tileBuffer; // Buffer for tileBufferTBO
	GLuint tileTexture;// Buffer for tileTextureTBO
	int numTiles;
	std::vector<std::string> filenames;
	TMType type = TMType::Normal;
	bool initialized = false;
};
#endif
