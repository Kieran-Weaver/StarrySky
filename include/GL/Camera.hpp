#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <GL/Window.hpp>
#include <util/Rect.hpp>
class Camera{
public:
	Camera(const Rect<float>& cbounds, const Rect<float>& sbounds, const Window& window);
	void Scroll(const glm::vec2& direction);
	void ScrollTo(const Rect<float>& object);
	glm::mat4 getVP();
	void reset();
	float scrollSpeed = 10.f;
private:
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 CachedVP;
	bool viewHasChanged = false;
	Rect<float> current_bounds;
	Rect<float> camera_bounds; // The camera cannot scroll past this
	Rect<float> scroll_bounds; // When you leave this, camera starts scrolling
};
#endif
