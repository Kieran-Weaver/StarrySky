#ifndef STARRYSKY_SPRITEBATCH_IMPL_H
#define STARRYSKY_SPRITEBATCH_IMPL_H
#include <GL/Shader.hpp>
#include <GL/Tilemap.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Window.hpp>
#include <GL/SpriteData.hpp>
#include <GL/Program.hpp>
#include <GL/Buffer.hpp>
#include <GL/VertexArray.hpp>
#include <unordered_map>
#include <file/JSONReader.hpp>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define OVERLAY 2
#define SPRITE3D 3
// This SpriteBatchImpl implementation requires OpenGL 3.3+
struct Sprite;
struct ImDrawData;
struct GLProgram{
	Program handle;
	VertexArray VAO = {};
	Buffer VBO = {};
	Buffer IBO = {};
	std::vector<uint32_t> extra_data;
};
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, const std::string& shaderfile);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders);
	void addMap(const std::string& id, const TileMap& tm);
	void Draw(Sprite& spr);
	void Draw(const Window& target);
private:
	struct TextureData{
		std::vector<SpriteData> sprites;         // Stencil off
		std::vector<SpriteData> stencilSprites;  // Stencil on
	};
	uint32_t MatrixID;
	void setStencil(bool new_state);
	void drawSprites(const std::vector<SpriteData>& data);
	void drawTileMap(const TileMap& tilemap, const uint32_t& UBOHandle);
	void setAttrib(GLProgram& currentProgram, JSONParser node, uint32_t start, uint32_t stride);
	void setAttrib(GLProgram& currentProgram, JSONParser node);
	std::vector<GLProgram> glPrograms;
	std::unordered_map<std::string, TileMap> m_Maps;
	std::unordered_map<uint32_t,TextureData> m_texData;
	TextureAtlas& m_atlas;
	std::array<uint32_t,3> stencil_state = {};
	JSONReader document;
};
#endif
