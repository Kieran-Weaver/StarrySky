#ifndef MovingEntity_hpp
#define MovingEntity_hpp
#include <GL/Sprite.h>
#include <GL/SpriteBatch.h>
#include "Map.hpp"
class MovingEntity {
public:
	glm::vec2 lastPosition;
	glm::vec2 position;
	glm::vec2 lastSpeed;
	glm::vec2 speed;
	int width;
	int height;
	bool pushedRightWall=false, pushesRightWall=false;
	bool pushedLeftWall=false, pushesLeftWall=false;
	bool wasOnGround=false, isOnGround=false;
	bool wasAtCeiling=false, isAtCeiling=false;
	bool onOneWayPlatform = false;
	bool dropFromOneWay = false;
	bool dead=false;
	Sprite spr;
	ObjMap& map;
	float maxFallSpeed = 600.0f;
public:
	MovingEntity(float x, float y, ObjMap& map);
	MovingEntity(float x, float y, float w, float h, ObjMap& map);
	void Update(float dt);
	void warpto(float x, float y);
	void warpto(float x, float y, ObjMap& newmap);
};

#endif
