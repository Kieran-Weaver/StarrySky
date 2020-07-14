#include <GL/Camera.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
Camera::Camera(const Rect<float>& cbounds, const Rect<float>& sbounds, const Window& window) : camera_bounds(cbounds), scroll_bounds(sbounds){
	int w, h;
	window.getWindowSize(w,h);
	current_bounds.width = w;
	current_bounds.height = h;
	this->reset();
}
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
void Camera::ScrollTo(const Rect<float>& object){
	if (!this->scroll_bounds.Contains(object)){
		float scrollX = 0.f, scrollY = 0.f;
		if (object.left <= this->scroll_bounds.left){
			scrollX = object.left - this->scroll_bounds.left;
		} else if ((object.left + object.width) >= (this->scroll_bounds.left + this->scroll_bounds.width)){
			scrollX = (object.left + object.width) - (this->scroll_bounds.left + this->scroll_bounds.width);
		}
		if (object.top <= this->scroll_bounds.top){
			scrollY = object.top - this->scroll_bounds.top;
		} else if ((object.top + object.height) >= (this->scroll_bounds.top + this->scroll_bounds.height)){
			scrollY = (object.top + object.height) - (this->scroll_bounds.top + this->scroll_bounds.height);
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
	current_bounds = Rect<float>(0.f,0.f,current_bounds.width,current_bounds.height);
	View = glm::mat4(1.0f);
	Projection = glm::ortho(0.f,current_bounds.width,current_bounds.height,0.f);
	viewHasChanged = true;
}
