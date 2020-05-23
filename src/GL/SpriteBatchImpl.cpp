#include <GL/SpriteBatchImpl.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <core/Editor.hpp>
#include <rapidjson/document.h>
#include <imgui/imgui.h>
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
	document.Parse(shaderdata.c_str());
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	int num_shaders = document["shaders.len"].GetInt();
	GLuint* VAOs = new GLuint[num_shaders];
	glGenVertexArrays(num_shaders,VAOs);
	glBindVertexArray(VAOs[0]);
	unsigned char* pixels;
	int width, height;


	if (this->loadPrograms(num_shaders,VAOs) == -1){
#ifndef NDEBUG
		std::cerr << "Not valid JSON" << std::endl;
#endif
		std::exit(1);
	}
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	for (int textureIndex = 0; textureIndex < m_atlas.m_num_textures; textureIndex++){
		this->m_texData[m_atlas.m_texture_handles[textureIndex]] = TextureData();
	}
	for (auto& i : glPrograms){
		glUseProgram(i.programHandle);
		glUniform1i(glGetUniformLocation(i.programHandle,"tex"),0);
		glUniformBlockBinding(i.programHandle, glGetUniformBlockIndex(i.programHandle, "VP"), 0); // Global VP data is at 0
	}

	glGenBuffers(1, &this->MatrixID);
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(TileMap), NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, this->MatrixID, 0, sizeof(glm::mat4) + sizeof(TileMap)); // Global VP data + TileMap data
	glBindVertexArray(glPrograms[TILEMAP].VAO);
	glUseProgram(glPrograms[TILEMAP].programHandle);
	glUniform1i(glGetUniformLocation(glPrograms[TILEMAP].programHandle,"tBuffer"),1);
	glUniform1i(glGetUniformLocation(glPrograms[TILEMAP].programHandle,"tTexture"),2);
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	delete[] VAOs;
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 255);
	setStencil(false);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDisable(GL_CULL_FACE);
	
	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "imgui_impl_starrysky";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	glPrograms[OVERLAY].extra_data.resize(4);
	glGenTextures(1, &glPrograms[OVERLAY].extra_data[0]);
	glGenBuffers(1, &glPrograms[OVERLAY].extra_data[1]);
	glPrograms[OVERLAY].extra_data[2] = glGetUniformLocation(glPrograms[OVERLAY].programHandle, "Tex");
	glPrograms[OVERLAY].extra_data[3] = glGetUniformLocation(glPrograms[OVERLAY].programHandle, "ProjMtx");

	glBindTexture(GL_TEXTURE_2D, glPrograms[OVERLAY].extra_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	io.Fonts->TexID = (ImTextureID)(intptr_t)glPrograms[OVERLAY].extra_data[0];
	std::array<std::array<GLuint, 2>, 3> imguiVertexAttrs = {{ {{sizeof(ImDrawVert), offsetof(ImDrawVert, pos)}}, {{sizeof(ImDrawVert), offsetof(ImDrawVert, uv)}}, {{sizeof(ImDrawVert), offsetof(ImDrawVert, col)}} }};
	auto& paramNode = document["shaders"][OVERLAY]["layout"];
	for (int i = 0; i < 3; i++){
		this->setAttrib(glPrograms[OVERLAY], paramNode[i], imguiVertexAttrs[i][1], imguiVertexAttrs[i][0]);
	}
}

SpriteBatchImpl::~SpriteBatchImpl(){
	for (auto& i : glPrograms){
		glDeleteVertexArrays(1,&i.VAO);
		glDeleteBuffers(1,&i.VBO);
		glDeleteProgram(i.programHandle);
	}
	glDeleteTextures(1, &glPrograms[OVERLAY].extra_data[0]);
	glDeleteBuffers(1, &glPrograms[OVERLAY].extra_data[1]);
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
}

void SpriteBatchImpl::Draw(Sprite& spr){
	GLuint& m_tex = spr.m_subtexture.m_texture;
	if (m_texData.find(m_tex) == m_texData.end()){
		m_texData[m_tex] = TextureData();
	}
	const GLRect2D& data = spr.render();
	if (spr.uses_stencil){
		m_texData[m_tex].stencilSprites.emplace_back(data);
	} else {
		m_texData[m_tex].sprites.emplace_back(data);
	}
}

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

	glUseProgram(glPrograms[OVERLAY].programHandle);
	glUniform1i(glPrograms[OVERLAY].extra_data[2], 0);
	glUniformMatrix4fv(glPrograms[OVERLAY].extra_data[3], 1, GL_FALSE, &ortho_projection[0][0]);

	glBindVertexArray(glPrograms[OVERLAY].VAO);
	glBindBuffer(GL_ARRAY_BUFFER, glPrograms[OVERLAY].VBO);
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

void SpriteBatchImpl::setAttrib(GLProgram& currentProgram, rapidjson::Value& node, GLuint start, GLuint stride){
	glBindVertexArray(currentProgram.VAO);
	std::string input_name = "";
	GLuint components = 2;
	GLuint type = GL_FLOAT;
	GLboolean normalized = false;
	input_name = node["name"].GetString();
	GLint inputHandle = node["location"].GetInt();
	components = node["components"].GetInt();	

	switch (node["type"].GetInt()){
	case 0:
		type = GL_FLOAT;
		break;
	case 1:
		type = GL_UNSIGNED_SHORT;
		break;
	case 2:
		type = GL_UNSIGNED_INT;
		break;
	case 3:
		type = GL_UNSIGNED_BYTE;
		break;
	default:
		type = GL_FLOAT;
		break;
	}

	normalized = node["norm"].GetBool();
	glBindAttribLocation(currentProgram.programHandle, inputHandle, input_name.c_str());
	glEnableVertexAttribArray(inputHandle);
	if (type == GL_FLOAT || normalized){
		glVertexAttribPointer(inputHandle,components,type,normalized,stride,reinterpret_cast<void*>(start));
	}else{
		glVertexAttribIPointer(inputHandle,components,type,stride,reinterpret_cast<void*>(start));
	}
}

void SpriteBatchImpl::setAttrib(GLProgram& currentProgram, rapidjson::Value& node){
	this->setAttrib(currentProgram, node, node["start"].GetInt(), node["stride"].GetInt());
}
int SpriteBatchImpl::loadPrograms(int num_shaders, GLuint* VAOs){
	rapidjson::Value& node = document["shaders"];
	for (int ind = 0; ind < num_shaders; ind++){
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
			this->setAttrib(currentProgram, parameterNode);
		}
	}
	return glPrograms.size();
}
void SpriteBatchImpl::Draw(const Window& target){
	target.makeCurrent();
	glUseProgram(glPrograms[SPRITE2D].programHandle);
	auto& ws = target.getWindowState();
	glBindBuffer(GL_UNIFORM_BUFFER, this->MatrixID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &ws.camera->getVP()[0][0]);
	glBindVertexArray(glPrograms[SPRITE2D].VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, glPrograms[SPRITE2D].VBO);
	glStencilFunc(GL_ALWAYS, 1, 255);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	for (auto& texturepair : m_texData){
		auto& currentTexData = texturepair.second;
		size_t buffersize = std::max(currentTexData.sprites.size(), currentTexData.stencilSprites.size())*sizeof(GLRect2D);
		glBindTexture(GL_TEXTURE_2D,texturepair.first);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(glPrograms[SPRITE2D].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, glPrograms[SPRITE2D].VBO);

		if (glPrograms[SPRITE2D].VBO_size < buffersize){
			glBufferData(GL_ARRAY_BUFFER, buffersize, NULL, GL_DYNAMIC_DRAW);
		}
		setStencil(true);
		this->drawSprites(currentTexData.stencilSprites);
		currentTexData.stencilSprites.clear();
		setStencil(false);
		this->drawSprites(currentTexData.sprites);
		currentTexData.sprites.clear();
	}
	setStencil(false);
	for (auto& tmap : m_Maps){
		drawTileMap(tmap.second, this->MatrixID);
	}
	glStencilFunc(GL_ALWAYS, 1, 255);
}
void SpriteBatchImpl::drawSprites(const std::vector<GLRect2D>& data){
	glBufferSubData(GL_ARRAY_BUFFER,0,data.size()*sizeof(GLRect2D),data.data());
	glDrawArrays(GL_POINTS,0,data.size());
}
void SpriteBatchImpl::drawTileMap(const TileMap& tilemap, const GLuint& UBO){
	glUseProgram(glPrograms[TILEMAP].programHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(glPrograms[TILEMAP].VAO);
	glBindBuffer(GL_ARRAY_BUFFER, glPrograms[TILEMAP].VBO);
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

	if (glPrograms[TILEMAP].VBO_size < 4){
		glBufferData(GL_ARRAY_BUFFER, 4, nullptr, GL_DYNAMIC_DRAW);
		glPrograms[TILEMAP].VBO_size = 4;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4, &tilemap.tileTextureTBO);
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
