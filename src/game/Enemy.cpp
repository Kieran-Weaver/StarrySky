#include <game/Enemy.hpp>
#include <game/Character.hpp>
#include <cmath>
#include <array>
#define GRAVITY 4000.0f
#define MAX_FALL_SPEED 600.0f
#define DEFPROSPEED 600.0f
Enemy::Enemy(float x, float y, const std::string& mainsprite, const std::string& hurtsprite, int hlth, TextureAtlas& atlas): m_me(x, y), m_atlas(atlas) {
	this->texs.reserve(2);
	std::array<std::string,2> fnames = {mainsprite,hurtsprite};
	for (auto& i : fnames){
		this->texs.emplace_back(atlas.findSubTexture(i));
	}
	this->m_spr.setTexture(this->texs[0]);
	Rect<float> tmpAABB = m_spr.getAABB();
	this->m_me.setSize(tmpAABB.right - tmpAABB.left, tmpAABB.bottom - tmpAABB.top);
	this->health = hlth;
	this->ihealth = hlth;
}
void Enemy::reset(){
	health = ihealth;
	if (flipped){
		m_spr.flip();
		flipped = false;
	}
	m_spr.setTexture(this->texs[0]);
}
void Enemy::Update(float dt, const CMap& map, const PPCollider& player) {
// Update states
	this->m_me.Update(dt, map);
	const MEState& state = this->m_me.getState();
	const Rect<float>& AABB = player.getAABB();
	float yspd = std::min(state.yspeed + (GRAVITY * dt), m_me.maxFallSpeed);
	float xspd = state.xspeed;

	if (invltimer()){
		if (player.collides(this->m_spr)) {
			invltimer.setTime(45);
			health--;
			if (textureIndex == 0) {
				m_spr.setTexture(texs[1]);
				textureIndex = 1;
			}
		} else {
			if (textureIndex != 0) {
				m_spr.setTexture(texs[0]);
				textureIndex = 0;
			}
		}
		if (health == 0) {
			dead = true;
		}
	}
	
	if (state.centerX() > ((AABB.right + AABB.left) / 2.f)){
		xspd = -walkSpeed;
		if (flipped){
			flipped = false;
			this->m_spr.flip();
		}
	}else{
		xspd = walkSpeed;
		if (!flipped){
			flipped = true;
			this->m_spr.flip();
		}
	}

	this->m_me.setSpeed(xspd, yspd);
	this->dead = this->dead || this->m_me.dead;
	this->m_spr.setPosition(state.centerX(), state.centerY());
}

void Enemy::warpto(float x, float y) {
	this->m_me.warpto(x, y);
}

const Rect<float>& Enemy::getAABB() const {
	return this->m_me.getState().hitbox;
}

bool Enemy::collides(const Sprite& other) const {
	return this->m_spr.collides(other);
}

void Enemy::Draw(SpriteBatch& frame) {
	if (!dead){
		frame.Draw(m_spr);
	}
}
