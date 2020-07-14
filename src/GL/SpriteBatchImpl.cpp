#include <GL/SpriteBatchImpl.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <file/PlainText.hpp>
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

	glGenBuffers(1, &this->MatrixID);
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(TileMap), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->MatrixID, 0, sizeof(glm::mat4) + sizeof(TileMap)); // Global VP data + TileMap data
	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].handle.bind();
	glUniform1i(glPrograms[TILEMAP].handle.getUniform("tBuffer"),1);
	glUniform1i(glPrograms[TILEMAP].handle.getUniform("tTexture"),2);
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	
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
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	glPrograms[OVERLAY].extra_data.resize(4);
	glGenTextures(1, &glPrograms[OVERLAY].extra_data[0]);
	glGenBuffers(1, &glPrograms[OVERLAY].extra_data[1]);
	glPrograms[OVERLAY].extra_data[2] = glPrograms[OVERLAY].handle.getUniform("Tex");
	glPrograms[OVERLAY].extra_data[3] = glPrograms[OVERLAY].handle.getUniform("ProjMtx");

	glBindTexture(GL_TEXTURE_2D, glPrograms[OVERLAY].extra_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
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

#ifndef NO_IMGUI
// Ported from submodules/imgui/examples/imgui_impl_opengl3.cpp
void SpriteBatchImpl::Draw(const ImDrawData* draw_data){
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);

	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float ortho_projection[4][4] =
	{
		{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
		{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
		{ 0.0f,         0.0f,        -1.0f,   0.0f },
		{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
	};

	glPrograms[OVERLAY].handle.bind();
	glUniform1i(glPrograms[OVERLAY].extra_data[2], 0);
	glUniformMatrix4fv(glPrograms[OVERLAY].extra_data[3], 1, GL_FALSE, &ortho_projection[0][0]);

	glPrograms[OVERLAY].VAO.bind();
	glPrograms[OVERLAY].VBO.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glPrograms[OVERLAY].extra_data[1]);
	glActiveTexture(GL_TEXTURE0);
	ImVec2 clip_off = draw_data->DisplayPos;
	ImVec2 clip_scale = draw_data->FramebufferScale;

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != NULL){
				// Resetting the render state does nothing
				if (pcmd->UserCallback != ImDrawCallback_ResetRenderState){
					pcmd->UserCallback(cmd_list, pcmd);
				}
			} else {
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					// Apply scissor/clipping rectangle
					glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

					// Bind texture, Draw
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
				}
			}
		}
	}
	// Restore render state
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
}
#endif

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
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &ws.camera->getVP()[0][0]);
	glPrograms[SPRITE2D].VAO.bind();
	glActiveTexture(GL_TEXTURE0);
	glPrograms[SPRITE2D].VBO.bind();
	glStencilFunc(GL_ALWAYS, 1, 255);
	glEnable(GL_DEPTH_TEST);
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
		drawTileMap(tmap.second, this->MatrixID);
	}
	glStencilFunc(GL_ALWAYS, 1, 255);
}
void SpriteBatchImpl::drawSprites(const std::vector<SpriteData>& data){
	if (!data.empty()){
		glPrograms[SPRITE2D].VBO.update(data.data(),data.size()*sizeof(SpriteData),0);
		glPrograms[SPRITE2D].VBO.bind();
		glDrawArrays(GL_POINTS,0,data.size());
	}
}
void SpriteBatchImpl::drawTileMap(const TileMap& tilemap, const GLuint& UBO){
	glPrograms[TILEMAP].handle.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].VBO.bind();
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(TileMap), &tilemap);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, tilemap.tileBufferTBO);
	glBindBuffer(GL_TEXTURE_BUFFER, tilemap.tileBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tilemap.tileBuffer);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_BUFFER, tilemap.tileTextureTBO);
	glBindBuffer(GL_TEXTURE_BUFFER, tilemap.tileTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R16UI, tilemap.tileTexture);

	glBufferData(GL_ARRAY_BUFFER, 4, &tilemap.tileTextureTBO, GL_DYNAMIC_DRAW);
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
void SpriteBatchImpl::addMap(const std::string& id, const TileMap& tm){
	this->m_Maps[id] = tm;
}
