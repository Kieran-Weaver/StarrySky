#include "SpriteBatch.h"
SpriteBatch::SpriteBatch(){
	spriteData.set_empty_key(nullptr);
	vertexData.set_empty_key(nullptr);
}
void SpriteBatch::Draw(Sprite* spr){
	spr->render();
	if (spriteData.find(spr->m_subtexture.m_texture) == spriteData.end()){
		spriteData[spr->m_subtexture.m_texture] = std::vector<Sprite*>();
		vertexData[spr->m_subtexture.m_texture] = std::vector<float>();
	}else if (std::find(spriteData[spr->m_subtexture.m_texture].begin(),spriteData[spr->m_subtexture.m_texture].end(),spr) == spriteData[spr->m_subtexture.m_texture].end()){
		spriteData[spr->m_subtexture.m_texture].emplace_back(spr);
		vertexData[spr->m_subtexture.m_texture].insert(vertexData[spr->m_subtexture.m_texture].end(),spr->cached_vtx_data,spr->cached_vtx_data+16);
	}
}
void SpriteBatch::Draw(GLFWwindow* target){
	glfwMakeContextCurrent(target);
	WindowState * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(target));
	glUniformMatrix4fv(ws->MatrixID,1,GL_FALSE,&ws->camera->getVP()[0][0]);
	for (auto& texturepair : spriteData){
		auto& spriteVector = texturepair.second;
		size_t spriteIndex = 0;
		size_t dataIndex = 0;
		size_t num_sprites = spriteVector.size();
		while ((spriteIndex < num_sprites)&&(spriteVector[spriteIndex]->m_drawn)&&(!spriteVector[spriteIndex]->m_changed)){
			spriteIndex++;
			dataIndex++;
		}
		size_t unchanged_sprites = dataIndex;
		for (;spriteIndex < num_sprites;spriteIndex++){
			if (!spriteVector[spriteIndex]->m_drawn){
				continue;
			}else{
				vertexData[texturepair.first].insert(vertexData[texturepair.first].begin()+(dataIndex*16),spriteVector[spriteIndex]->cached_vtx_data,spriteVector[spriteIndex]->cached_vtx_data+16);
				dataIndex++;
				spriteVector[spriteIndex]->m_drawn = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D,*texturepair.first);
		glBufferData(GL_ARRAY_BUFFER,vertexData[texturepair.first].size()*sizeof(float),vertexData[texturepair.first].data(),GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES,6*dataIndex,GL_UNSIGNED_SHORT,0);
		vertexData[texturepair.first].erase(vertexData[texturepair.first].begin()+(unchanged_sprites*16),vertexData[texturepair.first].end());
	}
}
