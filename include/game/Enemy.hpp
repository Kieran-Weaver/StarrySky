#ifndef STARRYSKY_ENEMY_HPP
#define STARRYSKY_ENEMY_HPP
#include <core/MovingEntity.hpp>
#include <core/Timer.hpp>
#include <GL/PPCollider.hpp>
#include <GL/Sprite.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/TextureAtlas.hpp>
class CMap;
class Enemy : public PPCollider {
public:
	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy(float x, float y, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas);
	void Update(float dt, const CMap& map, const PPCollider& player);
	void Draw(SpriteBatch& frame);
	void warpto(float x, float y);
	void reset();
	bool collides(const Sprite& Object2) const override;
	const Rect<float>& getAABB() const override;
public:
	float walkSpeed = 400.0f;
	bool dead = false;
private:
	MovingEntity m_me;
	Timer invltimer;
	int health;
	int ihealth;
	int textureIndex = 0;
	bool flipped = false;
	Sprite m_spr;
	TextureAtlas& m_atlas;
	std::vector<Texture> texs; //1, 2, 3
};
#endif
