#include "MovingEntity.hpp"

MovingEntity::MovingEntity(float x, float y, ObjMap& map): MovingEntity(x,y,0,0,map){}
MovingEntity::MovingEntity(float x, float y, int w, int h, ObjMap& map):
	m_position(x, y), m_width(w), m_height(h), m_lastPosition(x, y), m_map(map)
{}

void MovingEntity::warpto(float x, float y){
	m_lastPosition = glm::vec2(x,y);
	m_position = glm::vec2(x,y);
	m_speed = glm::vec2(0.0f,0.0f);
	isOnGround = false;
	wasOnGround = false;
}
void MovingEntity::Update(float dt) {
// Save current touching info
	this->m_lastPosition = this->m_position;
	this->m_lastSpeed = this->m_speed;
	this->pushedLeftWall = this->pushesLeftWall;
	this->pushedRightWall = this->pushesRightWall;
	this->wasOnGround = this->isOnGround;
	this->wasAtCeiling = this->isAtCeiling;
	this->m_position += (this->m_speed * dt);
	this->pushesRightWall=false;
	this->pushesLeftWall=false;
	this->isAtCeiling=false;
	this->isOnGround=false;
	this->onOneWayPlatform = false;
	Rect<float> hitbox = {m_position.x - m_width/2.f, m_position.y - m_height/2.f, m_width, m_height};
	Rect<float> lastHitbox = {m_lastPosition.x - m_width/2.f, m_lastPosition.y - m_height/2.f, m_width, m_height}; 
	for (auto& surf : m_map.surfaces){
		auto& i = surf.second;
		Rect<float> collision;
		if (hitbox.RIntersects(i.hitbox, collision) && !lastHitbox.Intersects(i.hitbox)){
			if ((i.flags & WallType::RWALL)&&(lastHitbox.left + lastHitbox.width <= i.hitbox.left)){
				m_position.x = i.hitbox.left - (m_width/2);
				m_speed.x = 0.0f;
				this->pushesRightWall=true;				
			}
			if ((i.flags & WallType::LWALL)&&(lastHitbox.left >= i.hitbox.left + i.hitbox.width)){
				m_position.x = i.hitbox.left + i.hitbox.width + (m_width/2);
				m_speed.x = 0.0f;
				this->pushesLeftWall=true;
			}
			if ((i.flags & WallType::CEIL) && (lastHitbox.top >= i.hitbox.top + i.hitbox.height)){
				m_position.y = i.hitbox.top + i.hitbox.height + (m_height/2);
				this->isAtCeiling=true;
				m_speed.y = 0;
			}
			if ((i.flags & WallType::FLOOR) || ((i.flags & WallType::ONEWAY)&&(this->dropFromOneWay))){
				if (lastHitbox.top + lastHitbox.height <= i.hitbox.top){
					m_position.y = i.hitbox.top-(m_height/2);
					this->isOnGround=true;
					this->onOneWayPlatform = (i.flags & WallType::ONEWAY);
					m_speed.y = 0;
				}
			}
		}
	}
	if (m_position.y > (m_map.position.y + m_map.height)){
		dead = true;
	}
	m_spr.setPosition(m_position);
}
