#ifndef SPRITE_H
#define SPRITE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#define WH_EPSILON 0.01
struct Sprite{
	Sprite();
	Sprite(const Texture& tex);
	void setTexture(const Texture& tex);
	void setPosition(const float& x, const float& y);
	void setPosition(const glm::vec2& pos);
	void rotate(const float& degrees);
	Rect<float> getAABB() const;
	void render();

	Texture m_subtexture;
	float cached_vtx_data[16]; // used for opengl's drawelements
	bool m_drawn = false;
	bool m_changed = true;
	glm::vec2 center; // world coordinates
	glm::mat4 m_model; // used for transforming the sprite beyond position: initially converts from [0,1],[0,1] to world coordinates
};
#endif
