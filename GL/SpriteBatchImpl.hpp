#ifndef STARRYSKY_SPRITEBATCH_IMPL_H
#define STARRYSKY_SPRITEBATCH_IMPL_H
#include "Shader.hpp"
#include "Helpers.hpp"
#include "Tilemap.hpp"
#include <gl.h>
#include <parallel_hashmap/phmap_fwd_decl.h>
#include "TextureAtlas.hpp"
#include "Window.hpp"
#include <rapidjson/document.h>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define SPRITE3D 2

using phmap::parallel_flat_hash_map;
struct Sprite;
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws, const std::string& shaderfile);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders,GLuint* VAOs);
	void addMap(const std::string& id, const TileMap& tm);
	void Draw(Sprite* spr);
	void Draw(const Window& target);
private:
	struct TextureData{
		std::vector<Sprite*> sprites;
		std::vector<GLRect2D> vertices;
	};
	struct GLProgram{
		Shader vxShader;
		Shader fgShader;
		Shader gsShader;
		GLuint programHandle;
		GLBuffer<uint16_t> ebo;
		GLuint VAO;
		GLuint VBO;
		GLuint VBO_size = 0;
	};
	int MatrixID;
	void setStencil(bool new_state);
	void drawTileMap(const TileMap& tilemap, const GLuint& UBOHandle);
	std::vector<GLProgram> glPrograms;
	std::vector<GLBuffer<float>> ubos;
	parallel_flat_hash_map<std::string, TileMap> m_Maps;
	parallel_flat_hash_map<GLuint,TextureData> m_texData;
	TextureAtlas& m_atlas;
	std::array<GLenum,3> stencil_state{GL_KEEP,GL_KEEP,GL_KEEP};
	rapidjson::Document document;
};
#endif
