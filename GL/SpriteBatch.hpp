#ifndef STARRYSKY_SPRITEBATCH_H
#define STARRYSKY_SPRITEBATCH_H
#include "Shader.hpp"
#include "Helpers.hpp"
#include "WindowState.h"
#include "TextureAtlas.hpp"
#include <gl.h>
#include <GLFW/glfw3.h>
#include <sparsehash/dense_hash_map>

// Rendering Modes
#define SPRITE2D 0
#define TILEMAP 1
#define SPRITE3D 2

struct Sprite;
struct Tilemap;
class SpriteBatch{
public:
	SpriteBatch(TextureAtlas& atlas, WindowState& ws);
	~SpriteBatch();
	int loadPrograms(const std::string& filename,int num_shaders,GLuint* VAOs);
	void Draw(Sprite* spr);
	void Draw(Tilemap* tm);
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
		}
		Shader vxShader;
		Shader fgShader;
		GLuint programHandle;
		GLBuffer<uint16_t> ebo;
		GLuint VAO;
	};
	std::vector<GLProgram> glPrograms;
	google::dense_hash_map<GLuint*,TextureData> m_texData;
	TextureAtlas& m_atlas;
};
#endif
