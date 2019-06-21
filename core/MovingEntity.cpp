#include "MovingEntity.hpp"

MovingEntity::MovingEntity(float x, float y, ObjMap& map): MovingEntity(x,y,0,0,map){}
MovingEntity::MovingEntity(float x, float y, float w, float h, ObjMap& map):
	lastPosition(x, y), position(x, y), width(w), height(h), map(map)
{}

void MovingEntity::warpto(float x, float y){
	lastPosition.x = x;
	lastPosition.y = y;
	position.x = x;
	position.y = y;
	speed = glm::vec2(0.0f,0.0f);
	isOnGround = false;
	wasOnGround = false;
}
void MovingEntity::Update(float dt) {
// Save current touching info
	this->lastPosition = this->position;
	this->lastSpeed = this->speed;
	this->pushedLeftWall = this->pushesLeftWall;
	this->pushedRightWall = this->pushesRightWall;
	this->wasOnGround = this->isOnGround;
	this->wasAtCeiling = this->isAtCeiling;
	this->position += (this->speed * dt);
	this->pushesRightWall=false;
	this->pushesLeftWall=false;
	this->isAtCeiling=false;
	this->isOnGround=false;
	this->onOneWayPlatform = false;
	for (auto& i : map.surfaces){
		switch (i.type){
		case WallType::RWALL:
			if ((lastPosition.x + (width/2)) <= i.x){
				if (i.x <= (position.x + (width/2))){
					if (((i.y + i.length) > (position.y-(height/2)))&&(i.y < (position.y+(height/2)))){
						position.x = i.x - (width/2);
						speed.x = 0.0f;
						this->pushesRightWall=true;
					}
				}
			}
			break;
		case WallType::LWALL:
			if ((lastPosition.x - (width/2)) >= i.x){
				if (i.x >= (position.x - (width/2))){
					if (((i.y+i.length) > (position.y-(height/2)))&&(i.y < (position.y+(height/2)))){
						position.x = i.x + (width/2);
						speed.x = 0.0f;
						this->pushesLeftWall=true;
					}
				}
			}
			break;
		case WallType::CEIL:
			if (lastPosition.y-(height/2) >= i.y){
				if ((i.y >= (position.y-(height/2)))){
					if ((((i.x+i.length) > (position.x-(width/2)))&&(i.x < (position.x+(width/2))))){
						position.y = i.y+(height/2);
						this->isAtCeiling=true;
						speed.y = 0;
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
			if (lastPosition.y+(height/2) <= i.y){
				if ((i.y <= (position.y+(height/2)))){
					if (((i.x+i.length) > (position.x-(width/2)))&&(i.x < (position.x+(width/2)))){
						position.y = i.y-(height/2);
						this->isOnGround=true;
						this->onOneWayPlatform = (i.type == WallType::ONEWAY);
						speed.y = 0;
					}
				}
			}
			break;
		}
	}
	if (position.y > (map.position.y + map.height)){
		dead = true;
	}
}
