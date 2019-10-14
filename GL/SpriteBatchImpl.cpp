#include "SpriteBatchImpl.hpp"
#include "Camera.hpp"
#include "Sprite.hpp"
#include "JSONHelper.hpp"
#include <sajson.h>
#include <iostream>
#include <string>
// TODO: Add tilemap functionality
SpriteBatchImpl::SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws) : m_atlas(atlas){
	std::string shaderdata = readWholeFile("data/shaders.json");
	char *sdata = new char[shaderdata.length()+1];
	std::strncpy(sdata, shaderdata.c_str(), shaderdata.length());
	document = new sajson::document(sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(shaderdata.length(), sdata)));
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	m_texData.set_empty_key(std::numeric_limits<GLuint>::max());
	sajson::value node = document->get_root();
	int num_shaders = get_int(node, "shaders.len");
	GLuint* VAOs = new GLuint[num_shaders];
	glGenVertexArrays(num_shaders,VAOs);
	glBindVertexArray(VAOs[0]);

	auto vbo_handles = new GLuint[m_atlas.m_num_textures];
	glGenBuffers(m_atlas.m_num_textures,vbo_handles);
	glBindBuffer(GL_ARRAY_BUFFER,vbo_handles[0]);
	if (this->loadPrograms(num_shaders,VAOs) == -1){
		std::cout << "Not valid JSON" << std::endl;
		return;
	}
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	genIndexBuffer<uint16_t>(0xffff,glPrograms[0].ebo);
	for (int textureIndex = 0; textureIndex < m_atlas.m_num_textures; textureIndex++){
		this->m_texData[m_atlas.m_texture_handles[textureIndex]] = TextureData();
		this->m_texData[m_atlas.m_texture_handles[textureIndex]].VBO = vbo_handles[textureIndex];
		glBindBuffer(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles[textureIndex]].VBO);
		glBufferData(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles[textureIndex]].vertices.size()*sizeof(Vertex),m_texData[m_atlas.m_texture_handles[textureIndex]].vertices.data(),GL_DYNAMIC_DRAW);
	}
	delete[] vbo_handles;
	for (auto& i : glPrograms){
		glUniform1i(glGetUniformLocation(i.programHandle,"tex"),0);
		glUniformBlockBinding(i.programHandle, glGetUniformBlockIndex(i.programHandle, "VP"), 0); // Global VP data is at 0
	}
	ubos.emplace_back();
	GLBuffer<float>& matrixbuffer = ubos.back();
	glBindBuffer(GL_UNIFORM_BUFFER, matrixbuffer.m_handle);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrixbuffer.m_handle, 0, 64); // Global VP data
	ws.MatrixID = ubos.size() - 1;
}
SpriteBatchImpl::~SpriteBatchImpl(){
	for (auto& i : m_texData){
		glDeleteBuffers(1,&i.second.VBO);
	}
	for (auto& i : glPrograms){
		glDeleteVertexArrays(1,&i.VAO);
		glDeleteProgram(i.programHandle);
		glDeleteBuffers(1,&i.ebo.m_handle);
	}
}
void SpriteBatchImpl::Draw(Sprite* spr){
	spr->render();
	GLuint m_tex = *spr->m_subtexture->m_texture;
	if (m_texData.find(m_tex) == m_texData.end()){
		m_texData[m_tex] = TextureData();
		glGenBuffers(1,&m_texData[m_tex].VBO);
	}
	if (std::find(m_texData[m_tex].sprites.begin(),m_texData[m_tex].sprites.end(),spr) == m_texData[m_tex].sprites.end()){
		m_texData[m_tex].sprites.emplace_back(spr);
		m_texData[m_tex].vertices.insert(m_texData[m_tex].vertices.end(),spr->cached_vtx_data.data(),spr->cached_vtx_data.data()+4);
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
int SpriteBatchImpl::loadPrograms(int num_shaders, GLuint* VAOs){
	sajson::value node = get_node(document->get_root(),"shaders");
	for (int ind = 0; ind < num_shaders; ind++){
		glPrograms.emplace_back();
		GLProgram& currentProgram = glPrograms.back();
		currentProgram.VAO = VAOs[ind];
		glBindVertexArray(currentProgram.VAO);
		sajson::value shaderNode = node.get_array_element(ind);

		currentProgram.fgShader.load(get_string(shaderNode, "fgFile"));
		currentProgram.vxShader.load(get_string(shaderNode, "vxFile"));
		bool usesGS = get_bool(shaderNode, "usesGS");
		if (usesGS){
			currentProgram.gsShader.load(get_string(shaderNode, "gsFile"));
			currentProgram.programHandle = CreateProgram(currentProgram.vxShader,currentProgram.gsShader,currentProgram.fgShader,get_string(shaderNode, "output"));
		}else{
			currentProgram.programHandle = CreateProgram(currentProgram.vxShader,currentProgram.fgShader,get_string(shaderNode, "output"));
		}
		sajson::value layoutNode = get_node(shaderNode,"layout");
		int arrayLength = layoutNode.get_length();
		for (int i = 0; i < arrayLength; i++){
			sajson::value parameterNode = layoutNode.get_array_element(i);
			std::string input_name = "";
			GLuint components = 2;
			GLuint type = GL_FLOAT;
			GLboolean normalized = false;
			GLuint stride = 0;
			GLuint start = 0;
			input_name = get_string(parameterNode, "name");
			
			components = get_int(parameterNode,"components");	
			
			switch (get_int(parameterNode, "type")){
			case 0:
				type = GL_FLOAT;
				break;
			case 1:
				type = GL_UNSIGNED_SHORT;
				break;
			default:
				type = GL_FLOAT;
				break;
			}
			
			normalized = get_bool(parameterNode, "norm");
			stride = get_int(parameterNode, "stride");
			start = get_int(parameterNode, "start");
			GLint inputHandle = glGetAttribLocation(glPrograms.back().programHandle, input_name.c_str());
			glEnableVertexAttribArray(inputHandle);
			if (start == 0){
				glVertexAttribPointer(inputHandle,components,type,normalized,stride,nullptr);
			}else{
				glVertexAttribPointer(inputHandle,components,type,normalized,stride,reinterpret_cast<void*>(start));
			}
		}
	}
	return glPrograms.size();
}
void SpriteBatchImpl::Draw(GLFWwindow* target){
	glfwMakeContextCurrent(target);
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	auto ws = static_cast<WindowState*>(glfwGetWindowUserPointer(target));
	glBindBuffer(GL_UNIFORM_BUFFER, ubos[ws->MatrixID].m_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &ws->camera->getVP()[0][0]);
	glBindVertexArray(glPrograms[SPRITE2D].VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glPrograms[SPRITE2D].ebo.m_handle);
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
				currentTexData.vertices.insert(currentTexData.vertices.end(),currentTexData.sprites[spriteIndex]->cached_vtx_data.data(),currentTexData.sprites[spriteIndex]->cached_vtx_data.data()+4);
				currentTexData.sprites[spriteIndex]->m_drawn = false;
				currentTexData.sprites[spriteIndex]->m_changed = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D,texturepair.first);
		glBindBuffer(GL_ARRAY_BUFFER,currentTexData.VBO);
		glBufferSubData(GL_ARRAY_BUFFER,skippedSprites*4*sizeof(Vertex),(currentTexData.vertices.size()-skippedSprites*4)*sizeof(Vertex),currentTexData.vertices.data()+(skippedSprites*4));
		glDrawElements(GL_TRIANGLES,6*(currentTexData.vertices.size()/4),GL_UNSIGNED_SHORT,nullptr);
	}
//	glUseProgram(glPrograms[TILEMAP].programHandle);
//	glBindVertexArray(glPrograms[TILEMAP].VAO);
}
void SpriteBatchImpl::ChangeMap(TileMap* tm){
	this->m_currentMap.reset(tm);
}
void TileMapDeleter::operator()(TileMap *p){
	delete p;
}
