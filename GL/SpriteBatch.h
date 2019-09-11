#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H
#include "Sprite.h"
#include "Shader.hpp"
#include "WindowState.h"
#include "TextureAtlas.h"
#include <gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <sparsehash/dense_hash_map>
class SpriteBatch{
public:
	SpriteBatch(TextureAtlas& atlas, WindowState& ws);
	~SpriteBatch();
	void Draw(Sprite* spr);
	void Draw(GLFWwindow* target);
private:
	struct TextureData{
		GLuint VBO;
		std::vector<Sprite*> sprites;
		std::vector<Vertex> vertices;
	};
	GLuint VAO;
	Shader vxShader;
	Shader fgShader;
	GLBuffer<uint16_t> ebo;
	GLuint shaderProgram;
	GLint posAttrib, texAttrib;
	google::dense_hash_map<GLuint*,TextureData> m_texData;
	TextureAtlas& m_atlas;
};
#endif
