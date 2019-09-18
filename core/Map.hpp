#ifndef STARRYSKY_MAP_HPP
#define STARRYSKY_MAP_HPP
#include <vector>
#include <string>
#include <gl.h>
#include <sparsehash/dense_hash_map>
#include <GL/TextureAtlas.hpp>
#include <GL/Sprite.hpp>
#include <GL/SpriteBatch.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
enum WallType { LWALL, RWALL, CEIL, FLOOR, ONEWAY  };
enum MenuType { SURFACE, LEDGE, TEXTURE, TILES, NONE };
struct Surface{
	float x;
	float y;
	float length;
	WallType type;
};
struct MapSprite{
	MapSprite(const glm::vec2& iPos, const std::string& fname, const Sprite& sprite) : iPosition(iPos), filename(fname), spr(sprite){}
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
	void addSurface(const Surface& wall);
	void addBGTexture(const glm::vec2& thisposition, const glm::vec2& toscale, const std::string& fname);
	void WriteToFile(const std::string& filename);
	glm::vec2 position;
	std::vector<Surface> surfaces;
	std::vector<glm::vec2> ledges; // top left corners
	int width = 1280;
	int height = 800;
	float ledgewidth = 8.f;
	float ledgeheight = 8.f;
	
private:
	TextureAtlas& m_atlas;
	std::vector<MapSprite> sprs;
};
#endif
