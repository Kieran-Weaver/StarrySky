#include "Sprite.h"
Sprite::Sprite(){
	this->m_subtexture = nullptr;
	this->topleft = glm::vec2(0,0);
	this->m_model = glm::mat4(1.0f);
}
Sprite::Sprite(Texture tex){
	this->topleft = glm::vec2(0,0);
	this->setTexture(tex);
}
void Sprite::setTexture(Texture tex){
	this->m_subtexture = tex;
	this->m_changed = true;
	this->m_model = glm::scale(glm::mat4(1.0f),glm::vec3(tex.width,tex.height,1));
}
void Sprite::setPosition(const float& x, const float& y){
	this->topleft.x = x;
	this->topleft.y = y;
	this->m_changed = true;
}
void Sprite::setPosition(const glm::vec2& pos){
	this->topleft = pos;
	this->m_changed = true;
}
void Sprite::render(){
	this->m_drawn = true;
	if (m_changed){
		glm::vec4 topleft_vtx = (this->m_model * glm::vec4(0.f,0.f,0.f,1.f))+glm::vec4(this->topleft,0.f,0.f);
		glm::vec4 topright_vtx = (this->m_model * glm::vec4(1.f,0.f,0.f,1.f))+glm::vec4(this->topleft,0.f,0.f);
		glm::vec4 botright_vtx = (this->m_model * glm::vec4(1.f,1.f,0.f,1.f))+glm::vec4(this->topleft,0.f,0.f);
		glm::vec4 botleft_vtx = (this->m_model * glm::vec4(0.f,1.f,0.f,1.f))+glm::vec4(this->topleft,0.f,0.f);
		this->cached_vtx_data[0] = topleft_vtx.x;
		this->cached_vtx_data[1] = topleft_vtx.y;
		this->cached_vtx_data[4] = topright_vtx.x;
		this->cached_vtx_data[5] = topright_vtx.y;
		this->cached_vtx_data[8] = botright_vtx.x;
		this->cached_vtx_data[9] = botright_vtx.y;
		this->cached_vtx_data[12]= botleft_vtx.x;
		this->cached_vtx_data[13]= botleft_vtx.y;
		if (this->m_subtexture.rotated == 0){
			this->cached_vtx_data[2] = this->m_subtexture.m_rect.left; // 1
			this->cached_vtx_data[3] = this->m_subtexture.m_rect.top;
			this->cached_vtx_data[6] = this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width; // 2
			this->cached_vtx_data[7] = this->m_subtexture.m_rect.top;
			this->cached_vtx_data[10]= this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width; // 3
			this->cached_vtx_data[11]= this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height;
			this->cached_vtx_data[14]= this->m_subtexture.m_rect.left;
			this->cached_vtx_data[15]= this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height; // 4
		}else{
			this->cached_vtx_data[6] = this->m_subtexture.m_rect.left; // 2
			this->cached_vtx_data[7] = this->m_subtexture.m_rect.top;
			this->cached_vtx_data[10] = this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width; // 3
			this->cached_vtx_data[11] = this->m_subtexture.m_rect.top;
			this->cached_vtx_data[14]= this->m_subtexture.m_rect.left + this->m_subtexture.m_rect.width; // 4
			this->cached_vtx_data[15]= this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height;
			this->cached_vtx_data[2]= this->m_subtexture.m_rect.left;
			this->cached_vtx_data[3]= this->m_subtexture.m_rect.top + this->m_subtexture.m_rect.height; // 1
		}
	}
}
