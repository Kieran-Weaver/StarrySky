#include "SpriteBatch.h"
#include <iostream>
SpriteBatch::SpriteBatch(){
	spriteData.set_empty_key(nullptr);
	vertexData.set_empty_key(nullptr);
}
void SpriteBatch::Draw(Sprite* spr){
	spr->render();
	if (spriteData.find(spr->m_subtexture.m_texture) == spriteData.end()){
		spriteData[spr->m_subtexture.m_texture] = std::vector<Sprite*>();
		vertexData[spr->m_subtexture.m_texture] = std::vector<float>();
		spriteData[spr->m_subtexture.m_texture].emplace_back(spr);
		vertexData[spr->m_subtexture.m_texture].insert(vertexData[spr->m_subtexture.m_texture].end(),spr->cached_vtx_data,spr->cached_vtx_data+16);
	}else if (std::find(spriteData[spr->m_subtexture.m_texture].begin(),spriteData[spr->m_subtexture.m_texture].end(),spr) == spriteData[spr->m_subtexture.m_texture].end()){
		spriteData[spr->m_subtexture.m_texture].emplace_back(spr);
		vertexData[spr->m_subtexture.m_texture].insert(vertexData[spr->m_subtexture.m_texture].end(),spr->cached_vtx_data,spr->cached_vtx_data+16);
	}
}
bool SpriteCMP(const Sprite* a, const Sprite* b){
	if (a->m_drawn && b->m_drawn){
		if (!a->m_changed && b->m_changed){
			return true;
		}else{
			return false;
		}
	}else{
		if (a->m_drawn){
			return true;
		}else{
			return false;
		}
	}
}
void SpriteBatch::Draw(GLFWwindow* target){
	glfwMakeContextCurrent(target);
	WindowState * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(target));
	glUniformMatrix4fv(ws->MatrixID,1,GL_FALSE,&ws->camera->getVP()[0][0]);
	for (auto& texturepair : spriteData){
		auto& spriteVector = texturepair.second;
		size_t spriteIndex = 0;
		size_t num_sprites = spriteVector.size();
		while ((spriteIndex<spriteVector.size())&&(spriteVector[spriteIndex]->m_drawn)&&(!spriteVector[spriteIndex]->m_changed)){
			spriteIndex++;
		}
		std::sort(spriteVector.begin()+spriteIndex,spriteVector.end(),SpriteCMP);
		if (vertexData[texturepair.first].size() > spriteIndex*16){
			vertexData[texturepair.first].erase(vertexData[texturepair.first].begin() + (spriteIndex*16),vertexData[texturepair.first].end());
		}
		for (;spriteIndex < num_sprites;spriteIndex++){
			if (!spriteVector[spriteIndex]->m_drawn){
				spriteVector[spriteIndex]->m_changed = false;
				continue;
			}else{
				vertexData[texturepair.first].insert(vertexData[texturepair.first].end(),spriteVector[spriteIndex]->cached_vtx_data,spriteVector[spriteIndex]->cached_vtx_data+16);
				spriteVector[spriteIndex]->m_drawn = false;
				spriteVector[spriteIndex]->m_changed = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D,*texturepair.first);
		glBufferData(GL_ARRAY_BUFFER,vertexData[texturepair.first].size()*sizeof(float),vertexData[texturepair.first].data(),GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES,6*(vertexData[texturepair.first].size()/16),GL_UNSIGNED_SHORT,0);
	}
}
