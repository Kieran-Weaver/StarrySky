#ifndef SPRITE_H
#define SPRITE_H
#include <glm/glm.hpp>
#include "Helpers.hpp"
#include "Rect.hpp"
#include <array>
#define WH_EPSILON 0.01
struct Texture;
struct Sprite{
	Sprite() noexcept;
	Sprite(const Texture* tex) noexcept;
	void setTexture(const Texture* tex);
	void setPosition(const float& x, const float& y);
	void setPosition(const glm::vec2& pos);
	void rotate(const float& degrees);
	void transform(const glm::mat4& matrix);
	Rect<float> getAABB() const;
	void render();

	Texture* m_subtexture;
	glm::vec2 center; // world coordinates
	bool m_drawn = false;
	bool m_changed = true;
	std::array<Vertex,4> cached_vtx_data; // used for opengl's drawelements
	glm::mat4 m_model; // used for transforming the sprite beyond position: initially converts from [0,1],[0,1] to world coordinates

private:
	std::array<glm::vec4,4> rectCorners = {glm::vec4(0.f,0.f,0.f,1.f), glm::vec4(1.f,0.f,0.f,1.f), glm::vec4(1.f,1.f,0.f,1.f), glm::vec4(0.f,1.f,0.f,1.f)}; 
};
#endif
