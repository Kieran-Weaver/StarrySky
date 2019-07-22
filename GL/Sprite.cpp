#include "Sprite.h"
Sprite::Sprite(){
	this->m_subtexture = nullptr;
	this->center = glm::vec2(0,0);
	this->m_model = glm::mat4(1.0f);
}
Sprite::Sprite(const Texture& tex){
	this->center = glm::vec2(0,0);
	this->setTexture(tex);
}
void Sprite::setTexture(const Texture& tex){
	this->m_subtexture = tex;
	this->m_changed = true;
	this->m_model = glm::translate(glm::mat4(1.f),glm::vec3(-0.5f,-0.5f,0.f));
	if (tex.rotated){
		this->m_model = glm::rotate(glm::mat4(1.f),glm::radians(-90.f),glm::vec3(0.f,0.f,1.f)) * this->m_model;
	}
	this->m_model = glm::scale(glm::mat4(1.f),glm::vec3(tex.width,tex.height,1))*this->m_model;
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
		glm::vec4 topleft_vtx = (this->m_model * glm::vec4(0.f,0.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
		glm::vec4 topright_vtx = (this->m_model * glm::vec4(1.f,0.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
		glm::vec4 botright_vtx = (this->m_model * glm::vec4(1.f,1.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
		glm::vec4 botleft_vtx = (this->m_model * glm::vec4(0.f,1.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
		this->cached_vtx_data[0].posX = topleft_vtx.x;
		this->cached_vtx_data[0].posY = topleft_vtx.y;
		this->cached_vtx_data[0].texX = this->m_subtexture.m_rect.left;
		this->cached_vtx_data[0].texY = this->m_subtexture.m_rect.top;
		this->cached_vtx_data[1].posX = topright_vtx.x;
		this->cached_vtx_data[1].posY = topright_vtx.y;
		this->cached_vtx_data[1].texX = this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width;
		this->cached_vtx_data[1].texY = this->m_subtexture.m_rect.top;
		this->cached_vtx_data[2].posX = botright_vtx.x;
		this->cached_vtx_data[2].posY = botright_vtx.y;
		this->cached_vtx_data[2].texX = this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width;
		this->cached_vtx_data[2].texY = this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height;
		this->cached_vtx_data[3].posX = botleft_vtx.x;
		this->cached_vtx_data[3].posY = botleft_vtx.y;
		this->cached_vtx_data[3].texX = this->m_subtexture.m_rect.left;
		this->cached_vtx_data[3].texY = this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height;
	}
}
bool compareX(const glm::vec4& lhs,const glm::vec4& rhs){
	return lhs.x < rhs.x;
}
bool compareY(const glm::vec4& lhs,const glm::vec4& rhs){
	return lhs.y < rhs.y;
}
Rect<float> Sprite::getAABB() const{
	glm::vec4 vertices[4];
	vertices[0] = (this->m_model * glm::vec4(0.f,0.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
	vertices[1] = (this->m_model * glm::vec4(1.f,0.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
	vertices[2] = (this->m_model * glm::vec4(1.f,1.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
	vertices[3] = (this->m_model * glm::vec4(0.f,1.f,0.f,1.f))+glm::vec4(this->center,0.f,0.f);
	auto xExtremes = std::minmax_element(vertices,vertices+4,compareX);
	auto yExtremes = std::minmax_element(vertices,vertices+4,compareY);
	return Rect<float>(xExtremes.first->x,yExtremes.first->y,xExtremes.second->x-xExtremes.first->x,yExtremes.second->y-yExtremes.first->y);
}
