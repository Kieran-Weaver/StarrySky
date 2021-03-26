#include <GL/Camera.hpp>
#include <util/Clib.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#define  CAM_XDIV 24.f
#define  CAM_YDIV 16.f

Camera::Camera(const Rect<float>& cbounds, const Rect<float>& sbounds, const Window& window) : camera_bounds(cbounds), scroll_bounds(sbounds){
	int w, h;
	window.getWindowSize(w,h);
	current_bounds = {0, 0, static_cast<float>(w), static_cast<float>(h)};
	this->reset();
}

void Camera::Scroll(const glm::vec2& direction){
	Rect<float> newbounds = current_bounds;
	newbounds.Translate(direction.x, direction.y);
	if (camera_bounds.Contains(newbounds)){
		current_bounds = newbounds;
		scroll_bounds.Translate(direction.x, direction.y);
		View = glm::translate(View, glm::vec3(-direction.x,-direction.y,0.0f));
		viewHasChanged = true;
	}
}

void Camera::ScrollTo(const Rect<float>& object){
	if (!this->scroll_bounds.Contains(object)){
		float centreX = ((scroll_bounds.left + scroll_bounds.right) / 2.f);
		float centreY = ((scroll_bounds.top + scroll_bounds.bottom) / 2.f);
		float objectX = ((object.left + object.right) / 2.f);
		float objectY = ((object.top + object.bottom) / 2.f);
		float scrollX = 0.f, scrollY = 0.f;
		
		if ((object.left <= scroll_bounds.left) || (object.right >= scroll_bounds.right)) {
			scrollX = (objectX - centreX) / CAM_XDIV;
		}
		
		if ((object.top <= scroll_bounds.top) || (object.bottom >= scroll_bounds.bottom)) {
			scrollY = (objectY - centreY) / CAM_YDIV;
		}
		
		this->Scroll({scrollX, scrollY});
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
	float scrollX = scroll_bounds.left - current_bounds.left;
	float scrollY = scroll_bounds.top - current_bounds.top;
	current_bounds = {0.f,0.f,current_bounds.right - current_bounds.left,current_bounds.bottom - current_bounds.top};
	scroll_bounds = {scrollX, scrollY, scrollX + scroll_bounds.right - scroll_bounds.left, scrollY + scroll_bounds.bottom - scroll_bounds.top};
	View = glm::mat4(1.0f);
	Projection = glm::ortho(0.f,current_bounds.right,current_bounds.bottom,0.f);
	viewHasChanged = true;
}
