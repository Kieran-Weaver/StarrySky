#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H
#include "Sprite.h"
#include "WindowState.h"
#include <gl.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <sparsehash/dense_hash_map>
class SpriteBatch{
	public:
	SpriteBatch();
	void Draw(Sprite* spr);
	void Draw(GLFWwindow* target);
	google::dense_hash_map<GLuint*,std::vector<Sprite*>> spriteData;
	google::dense_hash_map<GLuint*,std::vector<float>> vertexData;
};
#endif
