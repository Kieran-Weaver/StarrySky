#ifndef STARRYSKY_OBJMAP_HPP
#define STARRYSKY_OBJMAP_HPP
#include <core/CMap.hpp>
#include <file/JSONReader.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Tilemap.hpp>
#include <GL/Sprite.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <random>
class SpriteBatch;

struct MapSprite{
	glm::vec2 iPosition;
	std::string filename;
	Sprite spr;
};

class ObjMap {
public:
	ObjMap(const std::string& filename, TextureAtlas& atlas);
	void loadFromFile(const std::string& filename);
	void SetPosition(float x, float y);
	void Draw(SpriteBatch& frame);
	void addBGTexture(const glm::vec2& thisposition, const glm::mat2& transform, const std::string& fname);
	void WriteToFile(const std::string& filename);
	TileMap& getTM(const std::string& id);
	const CMap& getCollision(void);
	glm::vec2 position;
	std::vector<glm::vec2> ledges; // top left corners
	std::mt19937 rng;
	int width = 1280;
	int height = 800;
	float ledgewidth = 8.f;
	float ledgeheight = 8.f;
	
private:
	std::unordered_map<std::string, TileMap> internal_tms;
	std::unordered_map<int, MapSprite> sprs;
	bool tm_changed = false;
	TextureAtlas& m_atlas;
	CMap m_cmap;
	void loadTileMap(TileMap& tomodify, JSONParser tilemapNode);
};

#endif
