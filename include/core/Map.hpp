#ifndef STARRYSKY_MAP_HPP
#define STARRYSKY_MAP_HPP
#include <vector>
#include <string>
#include <random>
#include <core/RTree.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Sprite.hpp>
#include <GL/Tilemap.hpp>
#include <GL/SpriteBatch.hpp>
#include <glm/glm.hpp>
#include <file/JSONReader.hpp>
#include <visit_struct/visit_struct.hpp>
enum WallType { LWALL=1, RWALL=2, CEIL=4, FLOOR=8, ONEWAY=16  };
struct Surface{
	Rect<float> hitbox;
	int flags;
	Rect<float> getAABB() const{
		return hitbox;
	}
};
VISITABLE_STRUCT(Surface, hitbox, flags);
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
	uint32_t addSurface(const Surface& wall);
	void addBGTexture(const glm::vec2& thisposition, const glm::mat2& transform, const std::string& fname);
	void WriteToFile(const std::string& filename);
	std::vector<std::reference_wrapper<const Surface>> collide(const Rect<float>& rect) const;
	TileMap& getTM(const std::string& id);
	glm::vec2 position;
	FloatRTree surfaces;
	std::vector<glm::vec2> ledges; // top left corners
	std::mt19937 rng;
	int width = 1280;
	int height = 800;
	float ledgewidth = 8.f;
	float ledgeheight = 8.f;
	
private:
	std::unordered_map<std::string, TileMap> internal_tms;
	std::unordered_map<int, MapSprite> sprs;
	std::unordered_map<int, Surface> surfmap;
	bool tm_changed = false;
	TextureAtlas& m_atlas;
	void loadTileMap(TileMap& tomodify, JSONParser tilemapNode);
};
#endif
