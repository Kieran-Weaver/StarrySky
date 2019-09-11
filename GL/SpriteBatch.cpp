#include "SpriteBatch.h"
SpriteBatch::SpriteBatch(TextureAtlas& atlas, WindowState& ws) : m_atlas(atlas){
	m_texData.set_empty_key(nullptr);
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	auto vbo_handles = new GLuint[m_atlas.m_num_textures];
	glGenBuffers(m_atlas.m_num_textures,vbo_handles);
	glBindBuffer(GL_ARRAY_BUFFER,vbo_handles[0]);

	genIndexBuffer<uint16_t>(0xffff,ebo);
	
	vxShader = Shader(GL_VERTEX_SHADER);
	vxShader.load("data/shaders/default.vert");

	fgShader = Shader(GL_FRAGMENT_SHADER);
	fgShader.load("data/shaders/default.frag");

	shaderProgram = CreateProgram(vxShader,fgShader,"outColor");
	glUseProgram(shaderProgram);
	
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);

	texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib,2,GL_UNSIGNED_SHORT,GL_TRUE,sizeof(Vertex),reinterpret_cast<void*>((2*sizeof(float))));

	for (int textureIndex = 0; textureIndex < m_atlas.m_num_textures; textureIndex++){
		this->m_texData[m_atlas.m_texture_handles+textureIndex] = TextureData();
		this->m_texData[m_atlas.m_texture_handles+textureIndex].VBO = vbo_handles[textureIndex];
		glBindBuffer(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles+textureIndex].VBO);
		glBufferData(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles+textureIndex].vertices.size()*sizeof(Vertex),m_texData[m_atlas.m_texture_handles+textureIndex].vertices.data(),GL_DYNAMIC_DRAW);
	}
	delete[] vbo_handles;
	glUniform1i(glGetUniformLocation(shaderProgram,"tex"),0);
	ws.MatrixID = glGetUniformLocation(shaderProgram,"VP");
}
SpriteBatch::~SpriteBatch(){
	for (auto& i : m_texData){
		glDeleteBuffers(1,&i.second.VBO);
	}
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1,&ebo.handle);
	glDeleteVertexArrays(1,&VAO);
}
void SpriteBatch::Draw(Sprite* spr){
	spr->render();
	GLuint* m_tex = spr->m_subtexture.m_texture;
	if (m_texData.find(m_tex) == m_texData.end()){
		m_texData[m_tex] = TextureData();
		glGenBuffers(1,&m_texData[m_tex].VBO);
	}
	if (std::find(m_texData[m_tex].sprites.begin(),m_texData[m_tex].sprites.end(),spr) == m_texData[m_tex].sprites.end()){
		m_texData[m_tex].sprites.emplace_back(spr);
		m_texData[m_tex].vertices.insert(m_texData[m_tex].vertices.end(),spr->cached_vtx_data,spr->cached_vtx_data+4);
		glBindBuffer(GL_ARRAY_BUFFER,m_texData[m_tex].VBO);
		glBufferData(GL_ARRAY_BUFFER,m_texData[m_tex].vertices.size()*sizeof(Vertex),m_texData[m_tex].vertices.data(),GL_DYNAMIC_DRAW);
	}
}
bool SpriteCMP(const Sprite* a, const Sprite* b){
	if (a->m_drawn && b->m_drawn){
		return (!a->m_changed && b->m_changed);
	}else{
		return (a->m_drawn);
	}
}
void SpriteBatch::Draw(GLFWwindow* target){
	glfwMakeContextCurrent(target);
	auto ws = static_cast<WindowState*>(glfwGetWindowUserPointer(target));
	glUniformMatrix4fv(ws->MatrixID,1,GL_FALSE,&ws->camera->getVP()[0][0]);
	for (auto& texturepair : m_texData){
		auto& currentTexData = texturepair.second;
		size_t spriteIndex = 0;
		size_t num_sprites = currentTexData.sprites.size();
		while ((spriteIndex<currentTexData.sprites.size())&&(currentTexData.sprites[spriteIndex]->m_drawn)&&(!currentTexData.sprites[spriteIndex]->m_changed)){
			spriteIndex++;
		}
		size_t skippedSprites = spriteIndex;
		std::sort(currentTexData.sprites.begin()+spriteIndex,currentTexData.sprites.end(),SpriteCMP);
		if (currentTexData.vertices.size() > spriteIndex*4){
			currentTexData.vertices.erase(currentTexData.vertices.begin() + (spriteIndex*4),currentTexData.vertices.end());
		}
		for (;spriteIndex < num_sprites;spriteIndex++){
			if (!currentTexData.sprites[spriteIndex]->m_drawn){
				currentTexData.sprites[spriteIndex]->m_changed = false;
				continue;
			}else{
				currentTexData.vertices.insert(currentTexData.vertices.end(),currentTexData.sprites[spriteIndex]->cached_vtx_data,currentTexData.sprites[spriteIndex]->cached_vtx_data+4);
				currentTexData.sprites[spriteIndex]->m_drawn = false;
				currentTexData.sprites[spriteIndex]->m_changed = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D,*texturepair.first);
		glBindBuffer(GL_ARRAY_BUFFER,currentTexData.VBO);
		glBufferSubData(GL_ARRAY_BUFFER,skippedSprites*4*sizeof(Vertex),(currentTexData.vertices.size()-skippedSprites*4)*sizeof(Vertex),currentTexData.vertices.data()+(skippedSprites*4));
		glDrawElements(GL_TRIANGLES,6*(currentTexData.vertices.size()/4),GL_UNSIGNED_SHORT,nullptr);
	}
}
