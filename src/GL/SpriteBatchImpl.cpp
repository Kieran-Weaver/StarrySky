#include <GL/SpriteBatchImpl.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <file/PlainText.hpp>
#include <gl.h>
#ifndef NO_IMGUI
#include <imgui/imgui.h>
#include <core/Editor.hpp>
#endif
#include <string>
#ifndef NDEBUG
#include <iostream>
#endif
std::array<float,4> packmat2(const glm::mat2& matrix){
	return {matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]};
}
glm::mat2 unpackmat2(const std::array<float,4>& array){
	return {array[0], array[1], array[2], array[3]};
}
SpriteBatchImpl::SpriteBatchImpl(TextureAtlas& atlas, const std::string& shaderfile) : m_atlas(atlas){
	std::string shaderdata = readWholeFile(shaderfile);
	stencil_state = {GL_KEEP,GL_KEEP,GL_KEEP};
	document = {shaderdata.c_str()};
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	int num_shaders = document["shaders.len"];
	unsigned char* pixels;
	int width, height;
	if (this->loadPrograms(num_shaders) == -1){
#ifndef NDEBUG
		std::cerr << "Not valid JSON" << std::endl;
#endif
		std::exit(1);
	}
	glPrograms[SPRITE2D].handle.bind();

	for (auto& tex : m_atlas.m_texture_handles){
		this->m_texData[tex] = TextureData();
	}
	
	for (auto& i : glPrograms){
		i.handle.bind();
		glUniform1i(i.handle.getUniform("tex"),0);
		i.handle.bindUBO("VP", 0); // Global VP data is at 0
	}

	this->UBO = Buffer(GL_UNIFORM_BUFFER);
	this->UBO.update(nullptr, 0, sizeof(glm::mat4) + sizeof(UBOData));
	this->UBO.bind(0, 0, sizeof(glm::mat4) + sizeof(UBOData));
	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].handle.bind();
	glUniform1i(glPrograms[TILEMAP].handle.getUniform("tBuffer"),1);
	glUniform1i(glPrograms[TILEMAP].handle.getUniform("tTexture"),2);
	this->UBO.bind();
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 255);
	setStencil(false);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDisable(GL_CULL_FACE);
#ifndef NO_IMGUI
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "imgui_impl_starrysky";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	glPrograms[OVERLAY].extra_data.resize(4);
	glGenTextures(1, &glPrograms[OVERLAY].extra_data[0]);
	glGenBuffers(1, &glPrograms[OVERLAY].extra_data[1]);
	glPrograms[OVERLAY].extra_data[2] = glPrograms[OVERLAY].handle.getUniform("Tex");
	glPrograms[OVERLAY].extra_data[3] = glPrograms[OVERLAY].handle.getUniform("ProjMtx");

	glBindTexture(GL_TEXTURE_2D, glPrograms[OVERLAY].extra_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::array<GLint, 4> RASwizzle = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, RASwizzle.data());
#ifdef GL_UNPACK_ROW_LENGTH
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	io.Fonts->TexID = (ImTextureID)(intptr_t)glPrograms[OVERLAY].extra_data[0];
	std::array<std::array<GLuint, 2>, 3> imguiVertexAttrs = {{ {{sizeof(ImDrawVert), offsetof(ImDrawVert, pos)}}, {{sizeof(ImDrawVert), offsetof(ImDrawVert, uv)}}, {{sizeof(ImDrawVert), offsetof(ImDrawVert, col)}} }};
	auto paramNode = document["shaders"][OVERLAY]["layout"];
	for (int i = 0; i < 3; i++){
		this->setAttrib(glPrograms[OVERLAY], paramNode[i], imguiVertexAttrs[i][1], imguiVertexAttrs[i][0]);
	}
#endif
}

SpriteBatchImpl::~SpriteBatchImpl(){
#ifndef NO_IMGUI
	glDeleteTextures(1, &glPrograms[OVERLAY].extra_data[0]);
	glDeleteBuffers(1, &glPrograms[OVERLAY].extra_data[1]);
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
#endif
}

void SpriteBatchImpl::Draw(Sprite& spr){
	GLuint& m_tex = spr.m_subtexture.m_texture;
	if (m_texData.find(m_tex) == m_texData.end()){
		m_texData[m_tex] = {};
	}
	const auto& data = spr.render();
	if (spr.uses_stencil){
		m_texData[m_tex].stencilSprites.emplace_back(data);
	} else {
		m_texData[m_tex].sprites.emplace_back(data);
	}
}

void SpriteBatchImpl::setAttrib(GLProgram& currentProgram, JSONParser node, GLuint start, GLuint stride){
	currentProgram.VAO.bind();
	Attrib attr;
	std::string input_name = node["name"];
	attr.components = node["components"];
	attr.type = node["type"];
	attr.normalized = node["norm"];
	attr.location = node["location"];
	attr.start = start;
	attr.stride = stride;

	switch (attr.type){
	case 0:
		attr.type = GL_FLOAT;
		break;
	case 1:
		attr.type = GL_UNSIGNED_SHORT;
		break;
	case 2:
		attr.type = GL_UNSIGNED_INT;
		break;
	case 3:
		attr.type = GL_UNSIGNED_BYTE;
		break;
	default:
		attr.type = GL_FLOAT;
		break;
	}
	currentProgram.VAO.setAttrib(attr);
}

void SpriteBatchImpl::setAttrib(GLProgram& currentProgram, JSONParser node){
	this->setAttrib(currentProgram, node, node["start"], node["stride"]);
}

int SpriteBatchImpl::loadPrograms(int num_shaders){
	auto node = document["shaders"];
	for (int ind = 0; ind < num_shaders; ind++){
		glPrograms.emplace_back();
		GLProgram& currentProgram = glPrograms.back();
		currentProgram.VBO = Buffer(GL_ARRAY_BUFFER);
		currentProgram.IBO = Buffer(GL_ELEMENT_ARRAY_BUFFER);
		currentProgram.handle.load(node[ind]);
		currentProgram.VAO.bind();
		currentProgram.VBO.bind();
		auto layoutNode = node[ind]["layout"];
		for (auto& parameterNode : layoutNode.GetArray()){
			this->setAttrib(currentProgram, parameterNode);
		}
	}
	return glPrograms.size();
}

void SpriteBatchImpl::Draw(const Window& target){
	target.makeCurrent();
	glPrograms[SPRITE2D].handle.bind();
	auto& ws = target.getWindowState();
	this->UBO.bind();
	this->UBO.update(&ws.camera->getVP()[0][0], sizeof(glm::mat4), 0);
	glPrograms[SPRITE2D].VAO.bind();
	glActiveTexture(GL_TEXTURE0);
	glPrograms[SPRITE2D].VBO.bind();
	glStencilFunc(GL_ALWAYS, 1, 255);
	glDepthFunc(GL_LEQUAL);
	for (auto& texturepair : m_texData){
		auto& currentTexData = texturepair.second;
		size_t buffersize = std::max(currentTexData.sprites.size(), currentTexData.stencilSprites.size())*sizeof(decltype(currentTexData.sprites.back()));
		if (buffersize){
			glPrograms[SPRITE2D].VAO.bind();
			glBindTexture(GL_TEXTURE_2D,texturepair.first);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glPrograms[SPRITE2D].VBO.bind();
			setStencil(true);
			this->drawSprites(currentTexData.stencilSprites);
			currentTexData.stencilSprites.clear();
			setStencil(false);
			this->drawSprites(currentTexData.sprites);
			currentTexData.sprites.clear();
		}
	}
	setStencil(false);
	for (auto& tmap : m_Maps){
		drawTileMap(tmap, this->UBO);
	}
	this->m_Maps.clear();
	glStencilFunc(GL_ALWAYS, 1, 255);
#ifndef NO_IMGUI
	ImGui::Render();
	target.Draw(toDrawList(ImGui::GetDrawData(), glPrograms[OVERLAY].handle, glPrograms[OVERLAY].VBO, glPrograms[OVERLAY].IBO, glPrograms[OVERLAY].VAO));
#endif
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void SpriteBatchImpl::drawSprites(const std::vector<SpriteData>& data){
	if (!data.empty()){
		glPrograms[SPRITE2D].VBO.update(data.data(),data.size()*sizeof(SpriteData),0);
		glPrograms[SPRITE2D].VBO.bind();
		glDrawArrays(GL_POINTS,0,data.size());
	}
}
void SpriteBatchImpl::drawTileMap(const TileMap& tilemap, Buffer& UBO){
	uint32_t t = 0;

	glPrograms[TILEMAP].handle.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].VBO.bind();
	glPrograms[TILEMAP].VBO.update(&t, sizeof(t), 0);
	tilemap.bind(UBO, sizeof(glm::mat4));

	if (tilemap.type == TMType::Normal) {
		glStencilFunc(GL_ALWAYS, 1, 255);
	} else if (tilemap.type == TMType::Effect) {
		glStencilFunc(GL_EQUAL, 1, 255);
	}

	glDrawArraysInstanced(GL_POINTS, 0, 1, tilemap.drawn.size());
	glUseProgram(0);
	glStencilFunc(GL_ALWAYS, 1, 255);
}
void SpriteBatchImpl::setStencil(bool new_state){
	if (new_state) {
		if (stencil_state == std::array<GLenum,3>{GL_KEEP,GL_KEEP,GL_ZERO}) {
			stencil_state = {GL_KEEP, GL_KEEP, GL_REPLACE};
			glStencilOp(stencil_state[0], stencil_state[1], stencil_state[2]);
		}
	} else {
		if (stencil_state != std::array<GLenum,3>{GL_KEEP,GL_KEEP,GL_ZERO}) {
			stencil_state = {GL_KEEP, GL_KEEP, GL_ZERO};
			glStencilOp(stencil_state[0], stencil_state[1], stencil_state[2]);
		}
	}
}
void SpriteBatchImpl::Draw(const TileMap& tilemap){
	this->m_Maps.emplace_back(std::cref(tilemap));
}
