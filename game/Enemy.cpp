#include "Enemy.hpp"
#include "Character.hpp"
#include <cmath>
#define GRAVITY 4000.0f
#define MAX_FALL_SPEED 600.0f
#define DEFPROSPEED 600.0f
Enemy::Enemy(float x, float y, ObjMap& map, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas): MovingEntity(x, y, map), m_atlas(atlas) {
	this->texs.reserve(2);
	std::string fnames[] = {mainsprite,hurtsprite};
	for (int i=0;i<2;i++){
		this->texs.emplace_back(atlas.findSubTexture(fnames[i]));
	}
	this->spr.setTexture(this->texs[0]);
	Rect<float> tmpAABB = spr.getAABB();
	this->width = tmpAABB.width;
	this->height = tmpAABB.height;
	this->health = hlth;
	this->ihealth = hlth;
}
void Enemy::reset(){
	health = ihealth;
	spr.setTexture(this->texs[0]);
	flipped = false;
}
void Enemy::Update(float dt, Character* player) {
// Update states
	spr.setPosition(position);
	if (invltimer == 0){
		bool hit = false;
		if (player->swordout){
			if (m_atlas.PixelPerfectTest(player->spr2,spr)){
				hit = true;
			}
		}
		if (hit){
			invltimer=45;
			health--;
			if (textureIndex == 0){
				spr.setTexture(texs[1]);
				textureIndex = 1;
			}
		}else{
			if (textureIndex != 0){
				spr.setTexture(texs[0]);
				textureIndex = 0;
			}
		}
		if (health == 0){
			dead = true;
		}
	}else{
		invltimer--;
	}
	this->speed.y += GRAVITY * dt;
	this->speed.y = std::min(this->speed.y, maxFallSpeed);
	if (this->position.x > player->position.x){
		this->speed.x = -walkSpeed;
		if (flipped){
			flipped = false;
			this->spr.transform(this->flipped_mat);
		}
	}else{
		this->speed.x = walkSpeed;
		if (!flipped){
			flipped = true;
			this->spr.transform(this->flipped_mat);
		}
	}
	MovingEntity::Update(dt);
}
void Enemy::Draw(SpriteBatch& frame) {
	if (!dead){
		frame.Draw(&spr);
	}
}
