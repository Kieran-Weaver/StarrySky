#include <game/Enemy.hpp>
#include <game/Character.hpp>
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
	this->setSize(tmpAABB.right - tmpAABB.left, tmpAABB.bottom - tmpAABB.top);
	this->health = hlth;
	this->ihealth = hlth;
}
void Enemy::reset(){
	health = ihealth;
	if (flipped){
		m_spr.transform(this->flipped_mat);
	}
	m_spr.setTexture(this->texs[0]);
	flipped = false;
}
void Enemy::Update(float dt, Character* player) {
// Update states
	const MEState& state = this->getState();
	float yspd = std::min(state.yspeed + (GRAVITY * dt), maxFallSpeed);
	float xspd = state.xspeed;

	if (invltimer()){
		bool hit = false;
		if (player->swordout){
			if (this->m_spr.PPCollidesWith(player->m_spr2)){
				hit = true;
			}
		}
		if (hit){
			invltimer.setTime(45);
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
	}
	
	if (this->xpos() > player->xpos()){
		xspd = -walkSpeed;
		if (flipped){
			flipped = false;
			this->m_spr.transform(this->flipped_mat);
		}
	}else{
		xspd = walkSpeed;
		if (!flipped){
			flipped = true;
			this->m_spr.transform(this->flipped_mat);
		}
	}

	this->setSpeed(xspd, yspd);
	MovingEntity::Update(dt);
}
void Enemy::Draw(SpriteBatch& frame) {
	if (!dead){
		frame.Draw(m_spr);
	}
}
