#include <core/MEState.hpp>

void MEState::newFrame(float dt) {
	this->hitbox.Translate(this->xspeed * dt, this->yspeed * dt);
	this->atLeftWall  = false;
	this->atRightWall = false;
	this->atFloor     = false;
	this->atCeiling   = false;
	this->onOneWay    = false;
}

void MEState::setPosition(float x, float y) {
	float width  = this->hitbox.right - this->hitbox.left;
	float height = this->hitbox.bottom - this->hitbox.top;
	this->hitbox = { x, y, x + width, y + height };
}

float MEState::centerX(void) const {
	return (this->hitbox.right + this->hitbox.left) / 2.f;
}

float MEState::centerY(void) const {
	return (this->hitbox.bottom + this->hitbox.top) / 2.f;
}

float MEState::width(void) const {
	return this->hitbox.right - this->hitbox.left;
}

float MEState::height(void) const {
	return this->hitbox.bottom - this->hitbox.top;
}
