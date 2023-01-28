#ifndef STARRYSKY_SPRITEBATCH_IMPL_HPP
#define STARRYSKY_SPRITEBATCH_IMPL_HPP
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
#define SPRITE3D 3
// This SpriteBatchImpl implementation requires OpenGL 3.3+
struct Sprite;
struct ImDrawData;
struct GLProgram{
	Program handle;
	VertexArray VAO = {};
	Buffer VBO = {};
	Buffer IBO = {};
	Draw::IdxType idxType;
	std::vector<uint32_t> extra_data;
};
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, const std::string& shaderfile);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders);
	void Draw(TileMap& tilemap);
	void Draw(Sprite& spr);
	void EndFrame(const Window& target);
private:
	struct TextureData{
		std::vector<SpriteData> sprites;         // Stencil off
		std::vector<SpriteData> stencilSprites;  // Stencil on
	};
	void setStencil(bool new_state);
	void drawSprites(DrawCommand& drawComm, bool stencil);
	void drawTileMap(TileMap& tilemap, Buffer& UBOHandle);
	void setAttrib(GLProgram& currentProgram, JSONParser node, uint32_t start, uint32_t stride);
	void setAttrib(GLProgram& currentProgram, JSONParser node);
	DrawList m_drawlist;
	Buffer UBO;
	std::vector<GLProgram> glPrograms;
	std::vector<std::reference_wrapper<TileMap>> m_Maps;
	std::unordered_map<uint32_t,TextureData> m_texData;
	TextureAtlas& m_atlas;
	JSONReader document;
};
#endif
