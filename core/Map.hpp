#ifndef Map_hpp
#define Map_hpp
#include <fstream>
#include <vector>
#include <sstream>
#include <gl.h>
#include <sparsehash/dense_hash_map>
#include <GL/TextureAtlas.h>
#include <GL/Sprite.h>
#include <GL/SpriteBatch.h>
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
class ObjMap {
public:
	int width = 1280;
	int height = 800;
	float ledgewidth = 8.f;
	float ledgeheight = 8.f;
	glm::vec2 position;
public:
	ObjMap(const std::string& filename, TextureAtlas& atlas);
	void loadFromFile(const std::string& filename);
	void SetPosition(float x, float y);
	void Draw(SpriteBatch& frame);
	void addSurface(const Surface& wall);
	void addBGTexture(const glm::vec2& thisposition, const glm::vec2& toscale, const std::string& fname);
	void WriteToFile(std::string filename);
	std::vector<Sprite> sprs;
	std::vector<glm::vec2> iposs; //initial positions
	std::vector<std::string> texfilenames;
	std::vector<Surface> surfaces;
	std::vector<glm::vec2> ledges;
	TextureAtlas& m_atlas;
};
#endif
