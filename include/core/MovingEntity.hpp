#ifndef MovingEntity_hpp
#define MovingEntity_hpp
#include <GL/Sprite.hpp>
#include <core/Map.hpp>
struct MEState {
	void newFrame(float dt);
	void setPosition(float x, float y);
	
	Rect<float> hitbox = {};
	float xspeed = 0.f;
	float yspeed = 0.f;
	bool atLeftWall = false;
	bool atRightWall = false;
	bool atFloor = false;
	bool atCeiling = false;
	bool onOneWay = false;
};

class MovingEntity {
public:
/*	glm::vec2 m_position;
	glm::vec2 m_speed;
	int m_width;
	int m_height;*/
	float maxFallSpeed = 600.0f;
	bool dropFromOneWay = false;
	bool dead=false;
	Sprite m_spr;
	const glm::mat2 flipped_mat = glm::mat2(-1.f, 0.f, 0.f, 1.f);
public:
	MovingEntity(float x, float y, ObjMap& map);
	MovingEntity(float x, float y, int w, int h, ObjMap& map);
	void Update(float dt);
	const MEState& getState(void) const;
	void setSpeed(float x, float y);
	void setSize(float w, float h);
	void warpto(float x, float y);

	float xpos(void);
	float ypos(void);
	float width(void);
	float height(void);
private:
	MEState m_curr;
	MEState m_prev;
	ObjMap& m_map;
};

#endif
