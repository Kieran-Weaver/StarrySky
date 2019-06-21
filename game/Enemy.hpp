#ifndef ENEMY_HPP
#define ENEMY_HPP
#include "../core/MovingEntity.hpp"
#include "../GL/Sprite.h"
#include "../GL/SpriteBatch.h"
#include "../GL/TextureAtlas.h"
class Character;
class Enemy : public MovingEntity {
public:
	float walkSpeed = 400.0f;
	float maxFallSpeed = 600.0f;
	std::vector<Texture> texs; //1, 2, 3
	int textureIndex = 0;
	const glm::mat4 flipped_mat = glm::scale(glm::mat4(1.f),glm::vec3(-1.f,1.f,1.f));
	int invltimer = 0;
	int ihealth;
	int health;
	bool flipped = false;
	TextureAtlas& m_atlas;
public:
	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;
	Enemy(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas);
	void Update(float dt, Character* player);
	void Draw(SpriteBatch& frame);
	void reset();
};
#endif
