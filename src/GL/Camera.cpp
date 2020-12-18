#include <GL/Camera.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
Camera::Camera(const Rect<float>& cbounds, const Rect<float>& sbounds, const Window& window) : camera_bounds(cbounds), scroll_bounds(sbounds){
	int w, h;
	window.getWindowSize(w,h);
	current_bounds = {0.f, 0.f, w, h};
	this->reset();
}
void Camera::Scroll(const glm::vec2& direction){
	Rect<float> newbounds = current_bounds;
	newbounds.left += direction.x;
	newbounds.top += direction.y;
	newbounds.right += direction.x;
	newbounds.bottom += direction.y;
	if (camera_bounds.Contains(newbounds)){
		current_bounds = newbounds;
		View = glm::translate(View, glm::vec3(-direction.x,-direction.y,0.0f));
		viewHasChanged = true;
	}
}
void Camera::ScrollTo(const Rect<float>& object){
	if (!this->scroll_bounds.Contains(object)){
		float scrollX = 0.f, scrollY = 0.f;
		if (object.left <= this->scroll_bounds.left){
			scrollX = object.left - this->scroll_bounds.left;
		} else if (object.right >= this->scroll_bounds.right){
			scrollX = object.right - this->scroll_bounds.right;
		}
		if (object.top <= this->scroll_bounds.top){
			scrollY = object.top - this->scroll_bounds.top;
		} else if (object.bottom >= this->scroll_bounds.bottom){
			scrollY = object.bottom - this->scroll_bounds.bottom;
		}
		scrollX = std::max(std::fabs(scrollX), this->scrollSpeed) * sgn(scrollX);
		scrollY = std::max(std::fabs(scrollY), this->scrollSpeed) * sgn(scrollY);
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
	current_bounds = {0.f,0.f,current_bounds.right - current_bounds.left,current_bounds.bottom - current_bounds.top};
	View = glm::mat4(1.0f);
	Projection = glm::ortho(0.f,current_bounds.right,current_bounds.bottom,0.f);
	viewHasChanged = true;
}
