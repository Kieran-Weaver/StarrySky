#include "Bitmask.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include <glm/gtc/matrix_transform.hpp>
Sprite::Sprite() noexcept{
	this->m_subtexture = nullptr;
	this->center = glm::vec2(0,0);
	this->m_model = glm::mat4(1.0f);
}
Sprite::Sprite(const Texture* tex) noexcept{
	this->center = glm::vec2(0,0);
	this->setTexture(tex);
}
void Sprite::setTexture(const Texture* tex){
	this->m_subtexture = new Texture(tex);
	this->m_changed = true;
	this->m_model = glm::translate(glm::mat4(1.f),glm::vec3(-0.5f,-0.5f,0.f));
	if (tex->rotated != 0){
		this->m_model = glm::rotate(glm::mat4(1.f),glm::radians(-90.f),glm::vec3(0.f,0.f,1.f)) * this->m_model;
	}
	this->m_model = glm::scale(glm::mat4(1.f),glm::vec3(tex->width,tex->height,1))*this->m_model;
	for (int8_t i = 0; i < 4; i++){
		this->cached_vtx_data[i].texX = this->m_subtexture->m_rect.left;
		this->cached_vtx_data[i].texY = this->m_subtexture->m_rect.top;
		if (((i+1) & 0x02) != 0){ // 0 1 1 0
			this->cached_vtx_data[i].texX += this->m_subtexture->m_rect.width;
		}
		if (i >= 2){ // 0 0 1 1
			this->cached_vtx_data[i].texY += this->m_subtexture->m_rect.height;
		}
	}
}
void Sprite::setPosition(const float& x, const float& y){
	if ((x != center.x)||(y != center.y)){
		this->center.x = x;
		this->center.y = y;
		this->m_changed = true;
	}
}
void Sprite::setPosition(const glm::vec2& pos){
	this->setPosition(pos.x,pos.y);
}
void Sprite::rotate(const float& degrees){
	this->m_model = glm::rotate(glm::mat4(1.f),degrees,glm::vec3(0.f,0.f,1.f))*this->m_model;
	this->m_changed = true;
}
void Sprite::transform(const glm::mat4& matrix){
	this->m_model = matrix*this->m_model;
	this->m_changed = true;
}
void Sprite::render(){
	this->m_drawn = true;
	if (m_changed){
		for (int8_t i = 0; i < 4; i++){
			glm::vec2 vertex = glm::vec2(this->m_model * rectCorners[i]) + this->center;
			for (int8_t j=0;j<4;j++){
				this->cached_vtx_data[i].posX = vertex.x;
				this->cached_vtx_data[i].posY = vertex.y;
			}
		}
	}
}
bool compareX(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.x < rhs.x;
}
bool compareY(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.y < rhs.y;
}
Rect<float> Sprite::getAABB() const{
	glm::vec2 vertices[4];
	for (int8_t i=0;i<4;i++){
		vertices[i] = glm::vec2(this->m_model * rectCorners[i])+this->center;
	}
	auto xExtremes = std::minmax_element(vertices,vertices+4,compareX);
	auto yExtremes = std::minmax_element(vertices,vertices+4,compareY);
	return Rect<float>(xExtremes.first->x,yExtremes.first->y,xExtremes.second->x-xExtremes.first->x,yExtremes.second->y-yExtremes.first->y);
}
static Rect<float> Normalize(const Rect<uint16_t>& texrect){
	return Rect<float>(texrect.left/65536.f,texrect.top/65536.f,texrect.width/65536.f,texrect.height/65536.f);
}

bool Sprite::PPCollidesWith(const Sprite& Object2){
	Rect<float> Intersection; 
	const Rect<float> o1globalbounds = this->getAABB();
	const Rect<float> o2globalbounds = Object2.getAABB();
	if (o1globalbounds.RIntersects(o2globalbounds, Intersection)) {
		auto& mask1 = *this->m_subtexture->m_bitmask;
		auto& mask2 = *Object2.m_subtexture->m_bitmask;
		const Rect<float> o1m_rect = Normalize(this->m_subtexture->m_rect);
		const Rect<float> o2m_rect = Normalize(Object2.m_subtexture->m_rect);
		// Loop through our pixels
		const glm::mat4 o1t = glm::inverse(this->m_model);
		const glm::mat4 o2t = glm::inverse(Object2.m_model);
		const glm::mat4x2 o1t_subrect_to_pixel(
			this->m_subtexture->width,0,
			0,this->m_subtexture->height,
			0,0,
			o1m_rect.left*mask1.width,o1m_rect.top*mask1.height);
		const glm::mat4x2 o2t_subrect_to_pixel(
			Object2.m_subtexture->width,0,
			0,Object2.m_subtexture->height,
			0,0,
			o2m_rect.left*mask2.width,o2m_rect.top*mask2.height);
		for (int i = static_cast<int>(Intersection.left); i < static_cast<int>(Intersection.left+Intersection.width); i++) {
			for (int j = static_cast<int>(Intersection.top); j < static_cast<int>(Intersection.top+Intersection.height); j++) {

				glm::vec4 o1v = o1t*glm::vec4(i-this->center.x, j-this->center.y,0.f,1.f);
				glm::vec4 o2v = o2t*glm::vec4(i-Object2.center.x, j-Object2.center.y,0.f,1.f);

			// Make sure pixels fall within the sprite's subrect
				if (o1v.x > 0.f && o1v.y > 0.f && o2v.x > 0.f && o2v.y > 0.f &&
					o1v.x < 1.f && o1v.y < 1.f && o2v.x < 1.f && o2v.y < 1.f) {
					glm::ivec2 pixel1 = o1t_subrect_to_pixel * o1v;
					glm::ivec2 pixel2 = o2t_subrect_to_pixel * o2v;
					if (mask1.mask[pixel1.x+pixel1.y*mask1.width] && mask2.mask[pixel2.x+pixel2.y*mask2.width]){
						return true;
					}
				}
			}
		}
	}
	return false;
}