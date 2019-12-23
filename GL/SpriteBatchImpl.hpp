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
#include <rapidjson/document.h>
// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define SPRITE3D 2
struct Sprite;
class SpriteBatchImpl{
public:
	SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws, const std::string& shaderfile);
	~SpriteBatchImpl();
	int loadPrograms(int num_shaders,GLuint* VAOs);
	void ChangeMap(const TileMap& tm);
	void Draw(Sprite* spr);
	void Draw(GLFWwindow* target);
private:
	struct TextureData{
		std::vector<Sprite*> sprites;
		std::vector<GLRect2D> vertices;
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
		GLuint VBO;
		GLuint VBO_size = 0;
	};
	std::vector<GLProgram> glPrograms;
	std::vector<GLBuffer<float>> ubos;
	TileMap m_currentMap;
	google::dense_hash_map<GLuint,TextureData> m_texData;
	TextureAtlas& m_atlas;
	rapidjson::Document document;
};
#endif
