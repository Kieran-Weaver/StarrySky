#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "Rect.hpp"
class Camera{
public:
	Camera(const Rect<float>& cbounds, const Rect<float>& sbounds) : scroll_bounds(sbounds), camera_bounds(cbounds){}
	void Scroll(const glm::vec2& direction);
	glm::mat4 getVP();
	void reset();
	Rect<float> current_bounds = Rect<float>(0.f,0.f,1280.f,800.f);
	Rect<float> scroll_bounds; // When you leave this, camera starts scrolling
private:
	glm::mat4 View = glm::mat4(1.0f);
	glm::mat4 Projection = glm::ortho(0.f,1280.f,800.f,0.f);
	glm::mat4 CachedVP = Projection * View;
	Rect<float> camera_bounds; // The camera cannot scroll past this
	bool viewHasChanged = false;
};
#endif
