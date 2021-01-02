#ifndef SPRITE_H
#define SPRITE_H
#include <GL/PPCollider.hpp>
#include <GL/SpriteData.hpp>
#include <GL/Texture.hpp>
#include <util/Rect.hpp>
#include <glm/glm.hpp>
#include <array>
#include <memory>
#define WH_EPSILON 0.01
struct Sprite : public PPCollider {
	void setStencil(bool stencil_state);
	void setTexture(const Texture& tex);
	void setPosition(const float& x, const float& y, const float& z = 0.0f);
	void setPosition(const glm::vec2& pos);
	void setPosition(const glm::vec3& pos);
	void setColor(const std::array<uint8_t, 4> col);
	void rotate(const float& degrees);
	void transform(const glm::mat2& matrix);
	void flip(void);
	bool collides(const Sprite& Object2) const override;
	bool operator<(const Sprite& r) const{
		bool l_stencil = !this->uses_stencil;
		bool r_stencil = !r.uses_stencil;
		return std::tie(this->m_drawn, l_stencil, this->cached_vtx_data[0].vtxPos[2], this->m_changed) < std::tie(r.m_drawn, r_stencil, r.cached_vtx_data[0].vtxPos[2], r.m_changed);
	}
	const Rect<float>& getAABB() const override;
	const std::array<SpriteData,4>& render();
	glm::mat2 getMat2() const{
		return this->m_model;
	}
	Texture m_subtexture = {};
	bool m_drawn = false;
	bool m_changed = true;
	bool uses_stencil = false;
private:
	void renderAABB(void);
	Rect<float> cached_aabb = {};
	std::array<SpriteData,4> cached_vtx_data = {}; // used for opengl's drawelements
	glm::vec2 center = glm::vec2(0.f,0.f); // world coordinates
	glm::mat2 m_model = glm::mat2(1.0f); // used for transforming the sprite beyond position: initially converts from [0,1],[0,1] to world coordinates
};
#endif
