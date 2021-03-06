#include <core/MovingEntity.hpp>
#include <core/CMap.hpp>

MovingEntity::MovingEntity(float x, float y): MovingEntity(x,y,0,0){}
MovingEntity::MovingEntity(float x, float y, int w, int h) {
	this->m_curr.hitbox = { x, y, x + w, y + h };
	this->m_curr.xspeed = 0.f;
	this->m_curr.yspeed = 0.f;
	this->m_prev = this->m_curr;
}

void MovingEntity::setSize(float w, float h) {
	float x = this->m_curr.hitbox.left;
	float y = this->m_curr.hitbox.top;
	this->m_curr.hitbox = { x, y, x + w, y + h };
	this->m_prev = this->m_curr;
}


const MEState& MovingEntity::getState(void) const {
	return this->m_curr;
}

void MovingEntity::setSpeed(float x, float y) {
	this->m_curr.xspeed = x;
	this->m_curr.yspeed = y;
}

void MovingEntity::warpto(float x, float y) {
	this->m_curr.setPosition(x, y);
	this->m_curr.xspeed = 0.f;
	this->m_curr.yspeed = 0.f;
	this->m_curr.atFloor = false;
	this->m_prev.setPosition(x, y);
	this->m_prev.atFloor = false;
}

void MovingEntity::Update(float dt, const CMap& map) {
// Save current touching info
	this->m_prev = this->m_curr;
	this->m_curr.newFrame(dt);
	auto collisions = map.collide(this->m_curr.hitbox);
	float width = this->m_curr.width();
	float height = this->m_curr.height();
	
	for (auto& surf : collisions){
		const auto& i = surf.get();
		if ((i.flags & WallType::RWALL)&&(this->m_prev.hitbox.right <= i.hitbox.left)){
			this->m_curr.setPosition(i.hitbox.left - width, this->m_curr.hitbox.top);
			this->m_curr.xspeed = 0.f;
			this->m_curr.atRightWall = true;
		}
		if ((i.flags & WallType::LWALL)&&(this->m_prev.hitbox.left >= i.hitbox.right)){
			this->m_curr.setPosition(i.hitbox.right, this->m_curr.hitbox.top);
			this->m_curr.xspeed = 0.f;
			this->m_curr.atLeftWall = true;
		}
		if ((i.flags & WallType::CEIL) && (this->m_prev.hitbox.top >= i.hitbox.bottom)){
			this->m_curr.setPosition(this->m_curr.hitbox.left, i.hitbox.bottom);
			this->m_curr.yspeed = 0.f;
			this->m_curr.atCeiling = true;
		}
		if (((i.flags & WallType::FLOOR) || ((i.flags & WallType::ONEWAY)&&(!this->dropFromOneWay))) &&
			((this->m_prev.hitbox.bottom - 1.0f) <= i.hitbox.top)) {
			this->m_curr.setPosition(this->m_curr.hitbox.left, i.hitbox.top - height);
			this->m_curr.yspeed = 0.f;
			this->m_curr.atFloor = true;
			this->m_curr.onOneWay = (i.flags & WallType::ONEWAY);
		}
	}

	if (!map.getbounds().Intersects(this->m_curr.hitbox)){
		this->dead = true;
	}
}
