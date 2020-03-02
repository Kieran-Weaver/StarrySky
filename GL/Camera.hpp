#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include "Window.hpp"
#include "Rect.hpp"
class Camera{
public:
	Camera(const Rect<float>& cbounds, const Rect<float>& sbounds, const Window& window);
	void Scroll(const glm::vec2& direction);
	glm::mat4 getVP();
	void reset();
	Rect<float> current_bounds;
	Rect<float> scroll_bounds; // When you leave this, camera starts scrolling
private:
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 CachedVP;
	Rect<float> camera_bounds; // The camera cannot scroll past this
	bool viewHasChanged = false;
};
#endif
