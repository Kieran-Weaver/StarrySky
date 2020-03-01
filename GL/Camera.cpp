#include "Camera.hpp"
#include <algorithm>
#include <iosfwd>
#include "Rect.hpp"
void Camera::Scroll(const glm::vec2& direction){
	Rect<float> newbounds = current_bounds;
	newbounds.left += direction.x;
	newbounds.top += direction.y;
	if (camera_bounds.Contains(newbounds)){
		current_bounds = newbounds;
		View = glm::translate(View, glm::vec3(-direction.x,-direction.y,0.0f));
		viewHasChanged = true;
	}
}
glm::mat4 Camera::getVP(){
	if (viewHasChanged){
		CachedVP = Projection * View;
		viewHasChanged = false;
	}
	return CachedVP;
}
void Camera::reset(){
	current_bounds = Rect<float>(0.f,0.f,1280.f,720.f);
	View = glm::mat4(1.0f);
	Projection = glm::ortho(0.f,1280.f,720.f,0.f);
	viewHasChanged = true;
}
