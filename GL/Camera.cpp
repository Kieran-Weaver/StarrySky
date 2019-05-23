#include "Camera.hpp"
#include <iostream>
void Camera::Scroll(glm::vec2 direction){
	float hspd = glm::clamp(direction.x,-HSCROLLSPEED,HSCROLLSPEED);
	float vspd = glm::clamp(direction.y,-VSCROLLSPEED,VSCROLLSPEED);
	Rect<float> newbounds = current_bounds;
	newbounds.left += hspd;
	newbounds.top += vspd;
	if (camera_bounds.Contains(newbounds)){
		View = glm::translate(View, glm::vec3(-hspd,-vspd,0.0f));
		CachedVP = Projection * View;
	}
}
glm::mat4 Camera::getVP(){
	return CachedVP;
}
