#include "Sprite.h"
Sprite::Sprite() noexcept{
	this->m_subtexture = nullptr;
	this->center = glm::vec2(0,0);
	this->m_model = glm::mat4(1.0f);
}
Sprite::Sprite(const Texture& tex) noexcept{
	this->center = glm::vec2(0,0);
	this->setTexture(tex);
}
void Sprite::setTexture(const Texture& tex){
	this->m_subtexture = tex;
	this->m_changed = true;
	this->m_model = glm::translate(glm::mat4(1.f),glm::vec3(-0.5f,-0.5f,0.f));
	if (tex.rotated != 0){
		this->m_model = glm::rotate(glm::mat4(1.f),glm::radians(-90.f),glm::vec3(0.f,0.f,1.f)) * this->m_model;
	}
	this->m_model = glm::scale(glm::mat4(1.f),glm::vec3(tex.width,tex.height,1))*this->m_model;
	for (int8_t i = 0; i < 4; i++){
		this->cached_vtx_data[i].texX = this->m_subtexture.m_rect.left;
		this->cached_vtx_data[i].texY = this->m_subtexture.m_rect.top;
		if (((i+1) & 0x02) != 0){ // 0 1 1 0
			this->cached_vtx_data[i].texX += this->m_subtexture.m_rect.width;
		}
		if (i >= 2){ // 0 0 1 1
			this->cached_vtx_data[i].texY += this->m_subtexture.m_rect.height;
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
