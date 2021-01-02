#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H
#include <core/MEState.hpp>
class ObjMap;

class MovingEntity {
public:
	MovingEntity(float x, float y);
	MovingEntity(float x, float y, int w, int h);
	void Update(float dt, const ObjMap& map);
	const MEState& getState(void) const;
	void setSpeed(float x, float y);
	void setSize(float w, float h);
	void warpto(float x, float y);
public:
	float maxFallSpeed = 600.0f;
	bool dropFromOneWay = false;
	bool dead=false;
private:
	MEState m_curr;
	MEState m_prev;
};

#endif
