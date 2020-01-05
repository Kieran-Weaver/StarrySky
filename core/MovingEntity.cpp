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
	for (auto& surf : m_map.surfaces){
		auto& i = surf.second;
		switch (i.type){
		case WallType::RWALL:
			if ((m_lastPosition.x + (m_width/2)) <= i.x){
				if (i.x <= (m_position.x + (m_width/2))){
					if (((i.y + i.length) > (m_position.y-(m_height/2)))&&(i.y < (m_position.y+(m_height/2)))){
						m_position.x = i.x - (m_width/2);
						m_speed.x = 0.0f;
						this->pushesRightWall=true;
					}
				}
			}
			break;
		case WallType::LWALL:
			if ((m_lastPosition.x - (m_width/2)) >= i.x){
				if (i.x >= (m_position.x - (m_width/2))){
					if (((i.y+i.length) > (m_position.y-(m_height/2)))&&(i.y < (m_position.y+(m_height/2)))){
						m_position.x = i.x + (m_width/2);
						m_speed.x = 0.0f;
						this->pushesLeftWall=true;
					}
				}
			}
			break;
		case WallType::CEIL:
			if (m_lastPosition.y-(m_height/2) >= i.y){
				if ((i.y >= (m_position.y-(m_height/2)))){
					if ((((i.x+i.length) > (m_position.x-(m_width/2)))&&(i.x < (m_position.x+(m_width/2))))){
						m_position.y = i.y+(m_height/2);
						this->isAtCeiling=true;
						m_speed.y = 0;
					}
				}
			}
			break;
		case WallType::ONEWAY:
			if (this->dropFromOneWay){
				break;
			}
			__attribute__((fallthrough));
		case WallType::FLOOR:
			if (m_lastPosition.y+(m_height/2) <= i.y){
				if ((i.y <= (m_position.y+(m_height/2)))){
					if (((i.x+i.length) > (m_position.x-(m_width/2)))&&(i.x < (m_position.x+(m_width/2)))){
						m_position.y = i.y-(m_height/2);
						this->isOnGround=true;
						this->onOneWayPlatform = (i.type == WallType::ONEWAY);
						m_speed.y = 0;
					}
				}
			}
			break;
		}
	}
	if (m_position.y > (m_map.position.y + m_map.height)){
		dead = true;
	}
	m_spr.setPosition(m_position);
}
