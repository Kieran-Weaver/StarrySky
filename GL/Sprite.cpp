#include "Bitmask.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include "Mat2D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
void Sprite::setStencil(bool stencil_state){
	this->uses_stencil = stencil_state;
}
void Sprite::setTexture(const Texture& tex){
	this->m_subtexture = tex;
	this->m_changed = true;
	this->m_model = glm::mat2(1.f);
	if (tex.rotated != 0){
		this->m_model = RotMat(glm::radians(90.f)) * this->m_model;
	}
	this->m_model = ScaleMat(tex.width,tex.height)*this->m_model;
	this->cached_vtx_data.texRect[0] = this->m_subtexture.m_rect.left;
	this->cached_vtx_data.texRect[1] = this->m_subtexture.m_rect.top;
	this->cached_vtx_data.texRect[2] = this->m_subtexture.m_rect.width;
	this->cached_vtx_data.texRect[3] = this->m_subtexture.m_rect.height;
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
void Sprite::rotate(const float& radians){
	this->m_model = RotMat(radians)*this->m_model;
	this->m_changed = true;
}
void Sprite::transform(const glm::mat2& matrix){
	this->m_model = matrix*this->m_model;
	this->m_changed = true;
}
void Sprite::render(){
	this->m_drawn = true;
	if (m_changed){
		this->cached_vtx_data.sprPos[0] = center.x;
		this->cached_vtx_data.sprPos[1] = center.y;
		this->cached_vtx_data.packedMat[0] = this->m_model[0][0];
		this->cached_vtx_data.packedMat[1] = this->m_model[0][1];
		this->cached_vtx_data.packedMat[2] = this->m_model[1][0];
		this->cached_vtx_data.packedMat[3] = this->m_model[1][1];
	}
}
bool compareX(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.x < rhs.x;
}
bool compareY(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.y < rhs.y;
}
Rect<float> Sprite::getAABB() const{
	const std::array<glm::vec2, 4> rectCorners{{{-0.5f,-0.5f},{0.5f,-0.5f},{0.5f,0.5f},{-0.5f,0.5f}}};
	glm::vec2 vertices[4];
	for (int8_t i=0;i<4;i++){
		vertices[i] = glm::vec2(this->m_model * rectCorners[i])+this->center;
	}
	auto xExtremes = std::minmax_element(vertices,vertices+4,compareX);
	auto yExtremes = std::minmax_element(vertices,vertices+4,compareY);
	return Rect<float>(xExtremes.first->x,yExtremes.first->y,xExtremes.second->x-xExtremes.first->x,yExtremes.second->y-yExtremes.first->y);
}

bool Sprite::PPCollidesWith(const Sprite& Object2){
	Rect<float> Intersection; 
	const Rect<float> o1globalbounds = this->getAABB();
	const Rect<float> o2globalbounds = Object2.getAABB();
	if (o1globalbounds.RIntersects(o2globalbounds, Intersection)) {
		auto& mask1 = this->m_subtexture.m_bitmask;
		auto& mask2 = Object2.m_subtexture.m_bitmask;
		const Rect<float> o1m_rect = Normalize(this->m_subtexture.m_rect);
		const Rect<float> o2m_rect = Normalize(Object2.m_subtexture.m_rect);
		// Loop through our pixels
		const glm::mat2 o1t = glm::inverse(this->m_model);
		const glm::mat2 o2t = glm::inverse(Object2.m_model);
		const glm::mat2 o1t_subrect_to_pixel(
			o1m_rect.width * mask1.width,0,
			0,o1m_rect.height*mask1.height);
		const glm::vec2 o1t_subrect_center((o1m_rect.left+o1m_rect.width/2.f)*mask1.width,(o1m_rect.top+o1m_rect.height/2.f)*mask1.height);
		const glm::mat2 o2t_subrect_to_pixel(
			o2m_rect.width * mask2.width,0,
			0,o2m_rect.height*mask2.height);
		const glm::vec2 o2t_subrect_center((o2m_rect.left+o2m_rect.width/2.f)*mask2.width,(o2m_rect.top+o2m_rect.height/2.f)*mask2.height);
		for (int i = static_cast<int>(Intersection.left); i < static_cast<int>(Intersection.left+Intersection.width); i++) {
			for (int j = static_cast<int>(Intersection.top); j < static_cast<int>(Intersection.top+Intersection.height); j++) {

				glm::vec2 o1v = o1t*glm::vec2(i-this->center.x, j-this->center.y);
				glm::vec2 o2v = o2t*glm::vec2(i-Object2.center.x, j-Object2.center.y);

			// Make sure pixels fall within the sprite's subrect
				if (o1v.x > -0.5f && o1v.y > -0.5f && o2v.x > -0.5f && o2v.y > -0.5f &&
					o1v.x < 0.5f && o1v.y < 0.5f && o2v.x < 0.5f && o2v.y < 0.5f) {
					glm::ivec2 pixel1 = o1t_subrect_to_pixel * o1v + o1t_subrect_center;
					glm::ivec2 pixel2 = o2t_subrect_to_pixel * o2v + o2t_subrect_center;
					if ((*(mask1.mask))[pixel1.x+pixel1.y*mask1.width] && (*(mask2.mask))[pixel2.x+pixel2.y*mask2.width]){
						return true;
					}
				}
			}
		}
	}
	return false;
}
