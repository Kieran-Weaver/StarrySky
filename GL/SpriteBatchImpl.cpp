#include "SpriteBatchImpl.hpp"
#include "Camera.hpp"
#include "Sprite.hpp"
#include <rapidjson/document.h>
#include <iostream>
#include <string>
SpriteBatchImpl::SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws, const std::string& shaderfile) : m_atlas(atlas){
	std::string shaderdata = readWholeFile(shaderfile);
	document.Parse(shaderdata.c_str());
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	m_texData.set_empty_key(std::numeric_limits<GLuint>::max());
	int num_shaders = document["shaders.len"].GetInt();
	GLuint* VAOs = new GLuint[num_shaders];
	glGenVertexArrays(num_shaders,VAOs);
	glBindVertexArray(VAOs[0]);

	if (this->loadPrograms(num_shaders,VAOs) == -1){
		std::cout << "Not valid JSON" << std::endl;
		return;
	}
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	genIndexBuffer<uint16_t>(0xffff,glPrograms[SPRITE2D].ebo);
	for (int textureIndex = 0; textureIndex < m_atlas.m_num_textures; textureIndex++){
		this->m_texData[m_atlas.m_texture_handles[textureIndex]] = TextureData();
	}
	for (auto& i : glPrograms){
		glUniform1i(glGetUniformLocation(i.programHandle,"tex"),0);
		glUniformBlockBinding(i.programHandle, glGetUniformBlockIndex(i.programHandle, "VP"), 0); // Global VP data is at 0
	}
	ubos.emplace_back();
	GLBuffer<float>& matrixbuffer = ubos.back();
	glBindBuffer(GL_UNIFORM_BUFFER, matrixbuffer.m_handle);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(TileMap), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrixbuffer.m_handle, 0, sizeof(glm::mat4) + sizeof(TileMap)); // Global VP data + TileMap data
	glBindVertexArray(glPrograms[TILEMAP].VAO);
	glUseProgram(glPrograms[TILEMAP].programHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixbuffer.m_handle);
	glUniformBlockBinding(glPrograms[TILEMAP].programHandle, glGetUniformBlockIndex(glPrograms[TILEMAP].programHandle, "GSData"), 0);
	ws.MatrixID = ubos.size() - 1;
	delete[] VAOs;
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 255);
	setStencil(false);
}
SpriteBatchImpl::~SpriteBatchImpl(){
	for (auto& i : glPrograms){
		glDeleteVertexArrays(1,&i.VAO);
		glDeleteBuffers(1,&i.VBO);
		glDeleteProgram(i.programHandle);
		glDeleteBuffers(1,&i.ebo.m_handle);
	}
}
void SpriteBatchImpl::Draw(Sprite* spr){
	spr->render();
	GLuint& m_tex = spr->m_subtexture.m_texture;
	if (m_texData.find(m_tex) == m_texData.end()){
		m_texData[m_tex] = TextureData();
	}
	if (std::find(m_texData[m_tex].sprites.begin(),m_texData[m_tex].sprites.end(),spr) == m_texData[m_tex].sprites.end()){
		m_texData[m_tex].sprites.emplace_back(spr);
		m_texData[m_tex].vertices.emplace_back(spr->cached_vtx_data);
		if (m_texData[m_tex].vertices.size() * sizeof(GLRect2D) > glPrograms[SPRITE2D].VBO_size){
			glBindBuffer(GL_ARRAY_BUFFER, glPrograms[SPRITE2D].VBO);
			glBufferData(GL_ARRAY_BUFFER,m_texData[m_tex].vertices.size()*sizeof(GLRect2D),nullptr,GL_DYNAMIC_DRAW);
			glPrograms[SPRITE2D].VBO_size = m_texData[m_tex].vertices.size() * sizeof(GLRect2D);
		}
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
	rapidjson::Value& node = document["shaders"];
	for (rapidjson::SizeType ind = 0; ind < num_shaders; ind++){
		glPrograms.emplace_back();
		GLProgram& currentProgram = glPrograms.back();
		currentProgram.VAO = VAOs[ind];
		glBindVertexArray(currentProgram.VAO);
		glGenBuffers(1,&currentProgram.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, glPrograms[ind].VBO);
		rapidjson::Value& shaderNode = node[ind];

		currentProgram.fgShader = Shader(GL_FRAGMENT_SHADER, shaderNode["fgFile"].GetString());
		currentProgram.vxShader = Shader(GL_VERTEX_SHADER, shaderNode["vxFile"].GetString());
		bool usesGS = shaderNode["usesGS"].GetBool();
		if (usesGS){
			currentProgram.gsShader = Shader(GL_GEOMETRY_SHADER, shaderNode["gsFile"].GetString());
		}
		currentProgram.programHandle = CreateProgram(currentProgram.vxShader,currentProgram.gsShader,currentProgram.fgShader,shaderNode["output"].GetString());
		rapidjson::Value& layoutNode = shaderNode["layout"];
		for (auto& parameterNode : layoutNode.GetArray()){
			std::string input_name = "";
			GLuint components = 2;
			GLuint type = GL_FLOAT;
			GLboolean normalized = false;
			GLuint stride = 0;
			GLuint start = 0;
			input_name = parameterNode["name"].GetString();
			GLint inputHandle = parameterNode["location"].GetInt();
			components = parameterNode["components"].GetInt();	
			
			switch (parameterNode["type"].GetInt()){
			case 0:
				type = GL_FLOAT;
				break;
			case 1:
				type = GL_UNSIGNED_SHORT;
				break;
			case 2:
				type = GL_UNSIGNED_INT;
				break;
			default:
				type = GL_FLOAT;
				break;
			}
			
			normalized = parameterNode["norm"].GetBool();
			stride = parameterNode["stride"].GetInt();
			start = parameterNode["start"].GetInt();
			glBindAttribLocation(glPrograms.back().programHandle, inputHandle, input_name.c_str());
			glEnableVertexAttribArray(inputHandle);
			if (type == GL_FLOAT || normalized){
				glVertexAttribPointer(inputHandle,components,type,normalized,stride,reinterpret_cast<void*>(start));
			}else{
				glVertexAttribIPointer(inputHandle,components,type,stride,reinterpret_cast<void*>(start));
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
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &ws->camera->getVP()[0][0]);
	glBindVertexArray(glPrograms[SPRITE2D].VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, glPrograms[SPRITE2D].VBO);
	for (auto& texturepair : m_texData){
		auto& currentTexData = texturepair.second;
		size_t spriteIndex = 0;
		size_t num_sprites = currentTexData.sprites.size();
		while ((spriteIndex<currentTexData.sprites.size())&&(currentTexData.sprites[spriteIndex]->m_drawn)&&(!currentTexData.sprites[spriteIndex]->m_changed)){
			spriteIndex++;
		}
		size_t skippedSprites = spriteIndex;
		std::sort(currentTexData.sprites.begin()+spriteIndex,currentTexData.sprites.end(),SpriteCMP);
		if (currentTexData.vertices.size() > spriteIndex){
			currentTexData.vertices.erase(currentTexData.vertices.begin() + spriteIndex,currentTexData.vertices.end());
		}
		for (;spriteIndex < num_sprites;spriteIndex++){
			if (!currentTexData.sprites[spriteIndex]->m_drawn){
				currentTexData.sprites[spriteIndex]->m_changed = false;
				continue;
			}else{
				currentTexData.vertices.emplace_back(currentTexData.sprites[spriteIndex]->cached_vtx_data);
				currentTexData.sprites[spriteIndex]->m_drawn = false;
				currentTexData.sprites[spriteIndex]->m_changed = false;
			}
		}
		glBindTexture(GL_TEXTURE_2D,texturepair.first);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(glPrograms[SPRITE2D].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, glPrograms[SPRITE2D].VBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,currentTexData.vertices.size()*sizeof(GLRect2D),currentTexData.vertices.data());
		glDrawArrays(GL_POINTS,0,currentTexData.vertices.size());
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(glPrograms[TILEMAP].VAO);
	glBindBuffer(GL_ARRAY_BUFFER, glPrograms[TILEMAP].VBO);
	if (glPrograms[TILEMAP].VBO_size < m_currentMap.drawn.size()){
		glBufferData(GL_ARRAY_BUFFER, m_currentMap.drawn.size() * sizeof(Tile), nullptr, GL_DYNAMIC_DRAW);
		glPrograms[TILEMAP].VBO_size = m_currentMap.drawn.size();
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_currentMap.drawn.size() * sizeof(Tile), m_currentMap.drawn.data());
	glUseProgram(glPrograms[TILEMAP].programHandle);
	glDrawArrays(GL_POINTS, 0, 2);
	glUseProgram(0);
}
void SpriteBatchImpl::setStencil(bool new_state){
	if (new_state) {
		if (stencil_state == std::array<GLenum,3>{GL_KEEP,GL_KEEP,GL_KEEP}) {
			stencil_state = std::array<GLenum,3>{GL_KEEP, GL_KEEP, GL_REPLACE};
			glStencilOp(stencil_state[0], stencil_state[1], stencil_state[2]);
		}
	} else {
		if (stencil_state != std::array<GLenum,3>{GL_KEEP,GL_KEEP,GL_KEEP}) {
			stencil_state.fill(GL_KEEP);
			glStencilOp(stencil_state[0], stencil_state[1], stencil_state[2]);
		}
	}
}
void SpriteBatchImpl::ChangeMap(const TileMap& tm){
	this->m_currentMap = tm;
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(TileMap), &m_currentMap);
}
