#ifndef STARRYSKY_SPRITEBATCH_IMPL_H
#define STARRYSKY_SPRITEBATCH_IMPL_H
#include <GL/Shader.hpp>
#include <GL/Tilemap.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/Window.hpp>
#include <gl.h>
#include <parallel_hashmap/phmap_fwd_decl.h>
#include <rapidjson/document.h>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define OVERLAY 2
#define SPRITE3D 3
// This SpriteBatchImpl implementation requires OpenGL 3.3+
using phmap::parallel_flat_hash_map;
struct Sprite;
struct ImDrawData;
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, const std::string& shaderfile);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders,GLuint* VAOs);
	void addMap(const std::string& id, const TileMap& tm);
	void Draw(Sprite& spr);
	void Draw(const Window& target);
	void Draw(const ImDrawData* draw_data);
private:
	struct GLProgram{
		Shader vxShader;
		Shader fgShader;
		Shader gsShader;
		GLuint programHandle;
		GLuint VAO;
		GLuint VBO;
		GLuint VBO_size = 0;
		std::vector<GLuint> extra_data;
	};
	struct TextureData{
		std::vector<GLRect2D> sprites;         // Stencil off
		std::vector<GLRect2D> stencilSprites;  // Stencil on
	};
	GLuint MatrixID;
	void setStencil(bool new_state);
	void drawSprites(const std::vector<GLRect2D>& data);
	void drawTileMap(const TileMap& tilemap, const GLuint& UBOHandle);
	void setAttrib(GLProgram& currentProgram, rapidjson::Value& node, GLuint start, GLuint stride);
	void setAttrib(GLProgram& currentProgram, rapidjson::Value& node);
	std::vector<GLProgram> glPrograms;
	parallel_flat_hash_map<std::string, TileMap> m_Maps;
	parallel_flat_hash_map<GLuint,TextureData> m_texData;
	TextureAtlas& m_atlas;
	std::array<GLenum,3> stencil_state{GL_KEEP,GL_KEEP,GL_KEEP};
	rapidjson::Document document;
};
#endif
