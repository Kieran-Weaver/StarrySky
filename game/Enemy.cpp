#include "Enemy.hpp"
#include "Character.hpp"
#include <cmath>
#include <array>
#define GRAVITY 4000.0f
#define MAX_FALL_SPEED 600.0f
#define DEFPROSPEED 600.0f
Enemy::Enemy(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas): MovingEntity(x, y, map), m_atlas(atlas) {
	this->texs.reserve(2);
	std::array<std::string,2> fnames = {mainsprite,hurtsprite};
	for (auto& i : fnames){
		this->texs.emplace_back(atlas.findSubTexture(i));
	}
	this->m_spr.setTexture(this->texs[0]);
	Rect<float> tmpAABB = m_spr.getAABB();
	this->m_width = tmpAABB.width;
	this->m_height = tmpAABB.height;
	this->health = hlth;
	this->ihealth = hlth;
}
void Enemy::reset(){
	health = ihealth;
	m_spr.setTexture(this->texs[0]);
	flipped = false;
}
void Enemy::Update(float dt, Character* player) {
// Update states
	if (invltimer == 0){
		bool hit = false;
		if (player->swordout){
			if (m_atlas.PixelPerfectTest(player->m_spr2,m_spr)){
				hit = true;
			}
		}
		if (hit){
			invltimer=45;
			health--;
			if (textureIndex == 0){
				m_spr.setTexture(texs[1]);
				textureIndex = 1;
			}
		}else{
			if (textureIndex != 0){
				m_spr.setTexture(texs[0]);
				textureIndex = 0;
			}
		}
		if (health == 0){
			dead = true;
		}
	}else{
		invltimer--;
	}
	this->m_speed.y += GRAVITY * dt;
	this->m_speed.y = std::min(this->m_speed.y, maxFallSpeed);
	if (this->m_position.x > player->m_position.x){
		this->m_speed.x = -walkSpeed;
		if (flipped){
			flipped = false;
			this->m_spr.transform(this->flipped_mat);
		}
	}else{
		this->m_speed.x = walkSpeed;
		if (!flipped){
			flipped = true;
			this->m_spr.transform(this->flipped_mat);
		}
	}
	MovingEntity::Update(dt);
}
void Enemy::Draw(SpriteBatch& frame) {
	if (!dead){
		frame.Draw(&m_spr);
	}
}
