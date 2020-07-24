#include <util/Bitmask.hpp>
#include <util/Rect.hpp>
#include <util/Mat2D.hpp>
#include <GL/Sprite.hpp>
#include <GL/Texture.hpp>
#include <glm/gtc/matrix_transform.hpp>
void Sprite::setStencil(bool stencil_state){
	this->uses_stencil = stencil_state;
}
void Sprite::setTexture(const Texture& tex){
	this->m_changed = true;
	this->m_cached = false;
	if (this->m_subtexture.m_texture == 0) {
		this->m_model = glm::mat2(1.f);
	} else {
		if (m_subtexture.rotated != 0){
			this->m_model = this->m_model*RotMat(glm::radians(-90.f));
		}
		this->m_model = this->m_model*ScaleMat(1.f/m_subtexture.width, 1.f/m_subtexture.height);
	}
	this->m_subtexture = tex;
	this->m_model = this->m_model*ScaleMat(tex.width,tex.height);
	if (tex.rotated != 0){
		this->m_model = this->m_model*RotMat(glm::radians(90.f));
	}
}
void Sprite::setPosition(const float& x, const float& y, const float& z){
	if ((x != center.x)||(y != center.y)){
		this->center.x = x;
		this->center.y = y;
		this->m_changed = true;
		this->m_cached = false;
	}
	for (auto& i : this->cached_vtx_data){
		i.vtxPos[2] = z;
	}
}
void Sprite::setPosition(const glm::vec2& pos){
	this->setPosition(pos.x,pos.y);
}
void Sprite::setPosition(const glm::vec3& pos){
	this->setPosition(pos.x, pos.y, pos.z);
}
void Sprite::setColor(const std::array<uint8_t, 4> col){
	for (auto& i : this->cached_vtx_data){
		i.sprColor = col;
	}
}
void Sprite::rotate(const float& radians){
	this->m_model = RotMat(radians)*this->m_model;
	this->m_changed = true;
	this->m_cached = false;
}
void Sprite::transform(const glm::mat2& matrix){
	this->m_model = matrix*this->m_model;
	this->m_changed = true;
	this->m_cached = false;
}
const std::array<SpriteData, 4>& Sprite::render(){
	if (m_changed){
		const Rect<uint16_t>& texrect = this->m_subtexture.m_rect;
		// Bottom left, top left, top right, bottom right
		constexpr std::array<glm::vec2, 4> sprVertices = {{ {-0.5, 0.5}, {-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5} }};
		for (int i = 0 ; i < 4 ; i++){
			this->cached_vtx_data[i].texpos[0] = texrect.left + (i > 1) * texrect.width;
			this->cached_vtx_data[i].texpos[1] = texrect.top + ((i == 0) || (i == 3)) * texrect.height;
			glm::vec2 vert = this->center + (this->m_model * sprVertices[i]);
			this->cached_vtx_data[i].vtxPos[0] = vert.x;
			this->cached_vtx_data[i].vtxPos[1] = vert.y;
		}
		this->m_changed = false;
	}
	return this->cached_vtx_data;
}
static bool compareX(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.x < rhs.x;
}
static bool compareY(const glm::vec2& lhs,const glm::vec2& rhs){
	return lhs.y < rhs.y;
}
const Rect<float>& Sprite::getAABB(){
	if (!m_cached){
		this->renderAABB();
	}
	return cached_aabb;
}
void Sprite::renderAABB(){
	const std::array<glm::vec2, 4> rectCorners{{{-0.5f,-0.5f},{0.5f,-0.5f},{0.5f,0.5f},{-0.5f,0.5f}}};
	glm::vec2 vertices[4];
	for (int8_t i=0;i<4;i++){
		vertices[i] = glm::vec2(this->m_model * rectCorners[i])+this->center;
	}
	auto xExtremes = std::minmax_element(vertices,vertices+4,compareX);
	auto yExtremes = std::minmax_element(vertices,vertices+4,compareY);
	this->cached_aabb = Rect<float>(xExtremes.first->x,yExtremes.first->y,xExtremes.second->x-xExtremes.first->x,yExtremes.second->y-yExtremes.first->y);
	this->m_cached = true;
}

bool Sprite::PPCollidesWith(Sprite& Object2){
	Rect<float> Intersection; 
	const Rect<float>& o1globalbounds = this->getAABB();
	const Rect<float>& o2globalbounds = Object2.getAABB();
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
					if (mask1.get(pixel1.x,pixel1.y) && mask2.get(pixel2.x,pixel2.y)){
						return true;
					}
				}
			}
		}
	}
	return false;
}
