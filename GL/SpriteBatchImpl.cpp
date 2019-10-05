#include "SpriteBatchImpl.hpp"
#include "Camera.hpp"
#include "Sprite.hpp"
#include <simdjson/singleheader/simdjson.h>
#include <iosfwd>
// TODO: Add tilemap functionality
SpriteBatchImpl::SpriteBatchImpl(TextureAtlas& atlas, WindowState& ws) : m_atlas(atlas){
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	m_texData.set_empty_key(nullptr);
	int num_shaders = 1;
	GLuint* VAOs = new GLuint[num_shaders];
	glGenVertexArrays(num_shaders,VAOs);
	glBindVertexArray(VAOs[0]);

	auto vbo_handles = new GLuint[m_atlas.m_num_textures];
	glGenBuffers(m_atlas.m_num_textures,vbo_handles);
	glBindBuffer(GL_ARRAY_BUFFER,vbo_handles[0]);
	if (this->loadPrograms("data/shaders.json",num_shaders,VAOs) == -1){
		std::cout << "Not valid JSON" << std::endl;
		return;
	}
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	for (auto& i : glPrograms){
		genIndexBuffer<uint16_t>(0xffff,i.ebo);
	}
	for (int textureIndex = 0; textureIndex < m_atlas.m_num_textures; textureIndex++){
		this->m_texData[m_atlas.m_texture_handles+textureIndex] = TextureData();
		this->m_texData[m_atlas.m_texture_handles+textureIndex].VBO = vbo_handles[textureIndex];
		glBindBuffer(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles+textureIndex].VBO);
		glBufferData(GL_ARRAY_BUFFER,m_texData[m_atlas.m_texture_handles+textureIndex].vertices.size()*sizeof(Vertex),m_texData[m_atlas.m_texture_handles+textureIndex].vertices.data(),GL_DYNAMIC_DRAW);
	}
	delete[] vbo_handles;
	for (auto& i : glPrograms){
		glUniform1i(glGetUniformLocation(i.programHandle,"tex"),0);
	}
	ws.MatrixID = glGetUniformLocation(glPrograms[0].programHandle,"VP");
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
	GLuint* m_tex = spr->m_subtexture->m_texture;
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
int SpriteBatchImpl::loadPrograms(const std::string& filename, int num_shaders, GLuint* VAOs){
	simdjson::padded_string filedata = simdjson::get_corpus(filename);
	simdjson::ParsedJson pj = simdjson::build_parsed_json(filedata);
	if (!pj.is_valid()){
		return -1;
	}else{
		simdjson::ParsedJson::Iterator i(pj);
		if (i.move_to_key("shaders",7) && i.is_array()){
			for (int ind = 0; ind < num_shaders; ind++){
				i.down();
				if (i.is_object()){
					glPrograms.emplace_back();
					glPrograms.back().VAO = VAOs[ind];
					glBindVertexArray(glPrograms.back().VAO);
					if (i.move_to_key("vxFile",6)){
						glPrograms.back().vxShader.load(i.get_string());
						i.up();
					}else{
						throw std::invalid_argument("vxFile not found");
					}
					if (i.move_to_key("fgFile",6)){
						glPrograms.back().fgShader.load(i.get_string());
						i.up();
					}else{
						throw std::invalid_argument("fgFile not found");
					}
					if (i.move_to_key("output",6)){
						glPrograms.back().programHandle = CreateProgram(glPrograms.back().fgShader,glPrograms.back().vxShader,i.get_string());
						i.up();
					}else{
						throw std::invalid_argument("output not found");
					}
					if (i.move_to_key("layout",6)){
						i.down();
						do{
							std::string input_name = "";
							GLuint components = 2;
							GLuint type = GL_FLOAT;
							GLboolean normalized = false;
							GLuint stride = 0;
							GLuint start = 0;
							if (i.move_to_key("name",4)){
								input_name = i.get_string();
								i.up();
							}else{
								throw std::invalid_argument("name not found" + input_name);
							}
							if (i.move_to_key("components",10)){
								components = i.get_integer();
								i.up();
							}else{
								throw std::invalid_argument("components not found");
							}
							if (i.move_to_key("type",4)){
								switch(i.get_integer()){
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
								i.up();
							}
							if (i.move_to_key("norm",4)){
								normalized = i.is_true();
								i.up();
							}
							if (i.move_to_key("stride",6)){
								stride = i.get_integer();
								i.up();
							}
							if (i.move_to_key("start",5)){
								start = i.get_integer();
								i.up();
							}
							GLint inputHandle = glGetAttribLocation(glPrograms.back().programHandle, input_name.c_str());
							glEnableVertexAttribArray(inputHandle);
							if (start == 0){
								glVertexAttribPointer(inputHandle,components,type,normalized,stride,nullptr);
							}else{
								glVertexAttribPointer(inputHandle,components,type,normalized,stride,reinterpret_cast<void*>(start));
							}
						}while (i.next());
						i.up();
					}
					i.up();
					i.up();
				}
			}
			i.up();
		}
	}
	return glPrograms.size();
}
void SpriteBatchImpl::Draw(GLFWwindow* target){
	glfwMakeContextCurrent(target);
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	auto ws = static_cast<WindowState*>(glfwGetWindowUserPointer(target));
	glUniformMatrix4fv(ws->MatrixID,1,GL_FALSE,&ws->camera->getVP()[0][0]);
	glBindVertexArray(glPrograms[SPRITE2D].VAO);
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
		glBindTexture(GL_TEXTURE_2D,*texturepair.first);
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