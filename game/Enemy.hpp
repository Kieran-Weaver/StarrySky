#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <core/MovingEntity.hpp>
#include <GL/Sprite.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/TextureAtlas.hpp>
class Character;
class Enemy : public MovingEntity {
public:
	float walkSpeed = 400.0f;
	float maxFallSpeed = 600.0f;
	int textureIndex = 0;
	int invltimer = 0;
	int health;
	bool flipped = false;
public:
	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas);
	void Update(float dt, Character* player);
	void Draw(SpriteBatch& frame);
	void reset();
private:
	int ihealth;
	TextureAtlas& m_atlas;
	std::vector<const Texture*> texs; //1, 2, 3
};
#endif
