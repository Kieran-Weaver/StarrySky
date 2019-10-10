#ifndef STARRYSKY_SPRITEBATCH_IMPL_H
#define STARRYSKY_SPRITEBATCH_IMPL_H
#include "Shader.hpp"
#include "Helpers.hpp"
#include "Tilemap.hpp"
#include <gl.h>
#include <sparsehash/dense_hash_map>
#include "WindowState.h"
#include "TextureAtlas.hpp"
#include <GLFW/glfw3.h>
#include <sajson.h>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define SPRITE3D 2

struct Sprite;
struct TileMapDeleter{
	void operator()(TileMap *p);
};
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders,GLuint* VAOs);
	void Draw(Sprite* spr);
	void ChangeMap(TileMap* tm);
	void Draw(GLFWwindow* target);
private:
	struct TextureData{
		GLuint VBO;
		std::vector<Sprite*> sprites;
		std::vector<Vertex> vertices;
	};
	struct GLProgram{
		GLProgram(){
			this->vxShader = Shader(GL_VERTEX_SHADER);
			this->fgShader = Shader(GL_FRAGMENT_SHADER);
			this->gsShader = Shader(GL_GEOMETRY_SHADER);
		}
		Shader vxShader;
		Shader fgShader;
		Shader gsShader;
		GLuint programHandle;
		GLBuffer<uint16_t> ebo;
		GLuint VAO;
	};
	std::vector<GLProgram> glPrograms;
	std::unique_ptr<TileMap,TileMapDeleter> m_currentMap;
	google::dense_hash_map<GLuint*,TextureData> m_texData;
	TextureAtlas& m_atlas;
	sajson::document *document;
};
#endif
