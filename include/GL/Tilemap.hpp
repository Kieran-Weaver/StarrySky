#ifndef STARRYSKY_TILEMAP_H
#define STARRYSKY_TILEMAP_H
#include <cstdint>
#include <vector>
#include <array>
#include <visit_struct/visit_struct.hpp>
#include <GL/Buffer.hpp>
#include <file/JSONReader.hpp>
#include <GL/TextureAtlas.hpp>
using Tile = uint16_t;
enum TMType{
	Normal,
	Effect
};
struct UBOData{
	std::array<float,4> AffineT;
	std::array<float,4> Attrs;
	std::array<float,4> metadata;
	std::array<uint32_t,4> texData;
	uint32_t TBO1;
	uint32_t TBO2;
};
struct TileMap{
	TileMap() = default;
	TileMap(const JSONParser& node, const TextureAtlas& atlas);
	TileMap(const TileMap& other) = delete;
	TileMap& operator=(const TileMap& other) = delete;
	TileMap(TileMap&& other){
		*this = std::move(other);
	}
	TileMap& operator=(TileMap&& other);
	~TileMap();
	void bind(Buffer& UBO, uint32_t offset) const;
	void load(const JSONParser& node, const TextureAtlas& atlas);
	void loadTiles(); // Send tileData and drawn to GPU
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
	Buffer tileBuffer; // Buffer for tileBufferTBO
	Buffer tileTexture;// Buffer for tileTextureTBO
	int numTiles = 0;
	std::vector<std::string> filenames = {};
	TMType type = TMType::Normal;
	bool initialized = false;
};
VISITABLE_STRUCT(TileMap, AffineT, Attrs, metadata, texData, filenames, drawn, type);
#endif
