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
	Sprite() noexcept;
	Sprite(const Texture& tex) noexcept;
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
	glm::vec2 center; // world coordinates
	bool m_drawn = false;
	bool m_changed = true;
	bool uses_stencil = false;
	GLRect2D cached_vtx_data; // used for opengl's drawelements
	glm::mat2 m_model; // used for transforming the sprite beyond position: initially converts from [0,1],[0,1] to world coordinates
private:
	const std::array<glm::vec2, 4> rectCorners{{{-0.5f,-0.5f},{0.5f,-0.5f},{0.5f,0.5f},{-0.5f,0.5f}}};
};
#endif
