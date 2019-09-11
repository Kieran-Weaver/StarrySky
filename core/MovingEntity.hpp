#ifndef MovingEntity_hpp
#define MovingEntity_hpp
#include <GL/Sprite.h>
#include <GL/SpriteBatch.h>
#include "Map.hpp"
class MovingEntity {
public:
	glm::vec2 m_position;
	glm::vec2 m_speed;
	int m_width;
	int m_height;
	float maxFallSpeed = 600.0f;
	bool dead=false;
	Sprite m_spr;
public:
	MovingEntity(float x, float y, ObjMap& map);
	MovingEntity(float x, float y, int w, int h, ObjMap& map);
	void Update(float dt);
	void warpto(float x, float y);
protected:
	bool pushedRightWall=false, pushesRightWall=false;
	bool pushedLeftWall=false, pushesLeftWall=false;
	bool wasOnGround=false, isOnGround=false;
	bool wasAtCeiling=false, isAtCeiling=false;
	bool onOneWayPlatform = false;
	bool dropFromOneWay = false;
	glm::vec2 m_lastSpeed;
	glm::vec2 m_lastPosition;
	ObjMap& m_map;
	const glm::mat4 flipped_mat = glm::scale(glm::mat4(1.f),glm::vec3(-1.f,1.f,1.f));
};

#endif
