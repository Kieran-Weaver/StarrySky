#ifndef STARRYSKY_MESTATE_HPP
#define STARRYSKY_MESTATE_HPP
#include <util/Rect.hpp>

struct MEState {
	void newFrame(float dt);
	void setPosition(float x, float y);
	float centerX(void) const;
	float centerY(void) const;
	float width(void) const;
	float height(void) const;
	
	Rect<float> hitbox = {};
	float xspeed = 0.f;
	float yspeed = 0.f;
	bool atLeftWall = false;
	bool atRightWall = false;
	bool atFloor = false;
	bool atCeiling = false;
	bool onOneWay = false;
};

#endif
