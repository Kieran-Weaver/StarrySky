#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Rect.hpp"
class Camera{
public:
	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::ortho(0.f,1280.f,800.f,0.f);
	glm::mat4 CachedVP = Projection * View;
	Rect<float> current_bounds = Rect<float>(0.f,0.f,1280.f,800.f);
	Rect<float> camera_bounds; // The camera cannot scroll past this
	Rect<float> scroll_bounds; // When you leave this, camera starts scrolling
	float HScrollSpeed = 10.0f; // World coordinates
	float VScrollSpeed = 10.0f;
	bool viewHasChanged = false;
	Camera(Rect<float> cbounds, Rect<float> sbounds) : camera_bounds(cbounds), scroll_bounds(sbounds){}
	void Scroll(glm::vec2 direction);
	glm::mat4 getVP();
	void reset();
};
#endif
