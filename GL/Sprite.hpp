#ifndef SPRITE_H
#define SPRITE_H
#include <glm/glm.hpp>
#include <memory>
#include "Helpers.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include <array>
#define WH_EPSILON 0.01
struct Sprite{
	void setStencil(bool stencil_state);
	void setTexture(const Texture& tex);
	void setPosition(const float& x, const float& y);
	void setPosition(const glm::vec2& pos);
	void rotate(const float& degrees);
	void transform(const glm::mat2& matrix);
	bool PPCollidesWith(const Sprite& Object2);
	Rect<float> getAABB() const;
	void render();

	Texture m_subtexture;
	glm::vec2 center = glm::vec2(0.f,0.f); // world coordinates
	bool m_drawn = false;
	bool m_changed = true;
	bool uses_stencil = false;
	GLRect2D cached_vtx_data = GLRect2D(); // used for opengl's drawelements
	glm::mat2 m_model = glm::mat2(1.0f); // used for transforming the sprite beyond position: initially converts from [0,1],[0,1] to world coordinates
};
#endif
