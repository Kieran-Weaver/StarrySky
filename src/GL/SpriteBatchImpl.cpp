#include <GL/SpriteBatchImpl.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <file/PlainText.hpp>
#include <gl.h>
#ifndef NO_IMGUI
#include <imgui/imgui.h>
#endif
#include <string>
#ifndef NDEBUG
#include <iostream>
#endif
#include <numeric>
std::array<float,4> packmat2(const glm::mat2& matrix){
	return {matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]};
}
glm::mat2 unpackmat2(const std::array<float,4>& array){
	return {array[0], array[1], array[2], array[3]};
}
// Generates an index buffer for drawing n quads
template<typename T>
void genBufImpl(Buffer& IBO, uint16_t n){
	constexpr std::array<uint8_t, 6> vertices = { 0, 1, 2, 0, 2, 3 };
	std::vector<T> data;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < 6; j++){
			data.emplace_back(vertices[j] + (i * 4));
		}
	}
	IBO.update(data, 0);
}
Draw::IdxType genIdxBuffer(Buffer& IBO, uint16_t n){
	if (n > 64){
		genBufImpl<uint16_t>(IBO, n);
		return Draw::Short;
	} else {
		genBufImpl<uint8_t>(IBO, n);
		return Draw::Byte;
	}
}
SpriteBatchImpl::SpriteBatchImpl(TextureAtlas& atlas, const std::string& shaderfile) : m_atlas(atlas){
	std::string shaderdata = readWholeFile(shaderfile);
	document = {shaderdata.c_str()};
	int num_shaders = document["shaders.len"];
	unsigned char* pixels;
	int width, height;
	if (this->loadPrograms(num_shaders) == -1){
#ifndef NDEBUG
		std::cerr << "Not valid JSON" << std::endl;
#endif
		std::exit(1);
	}

	for (auto& tex : m_atlas.m_texture_handles){
		this->m_texData[tex] = TextureData();
	}
	
	for (auto& i : glPrograms){
		i.handle.bind();
		i.handle.setUniform("tex",0);
		i.handle.bindUBO("VP", 0); // Global VP data is at 0
	}

	this->UBO = Buffer(GL_UNIFORM_BUFFER);
	this->UBO.update(nullptr, 0, sizeof(glm::mat4) + sizeof(UBOData));
	this->UBO.bind(0, 0, sizeof(glm::mat4) + sizeof(UBOData));
	this->UBO.bind();

	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].handle.bind();
	glPrograms[TILEMAP].handle.setUniform("tBuffer",1);
	glPrograms[TILEMAP].handle.setUniform("tTexture",2);

	ConfCommand conf;
	conf.enable_flags[GL_DEPTH_TEST] = true;
	conf.enable_flags[GL_STENCIL_TEST] = true;
	conf.enable_flags[GL_BLEND] = true;
	conf.enable_flags[GL_FRAMEBUFFER_SRGB] = true;
	conf.enable_flags[GL_CULL_FACE] = false;
	conf.depth_func = GL_LEQUAL;
	conf.stencil_func = {GL_ALWAYS, 1, 255};
	conf.stencil_op = {GL_KEEP, GL_KEEP, GL_ZERO};
	conf.blend_func = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
	this->m_drawlist.emplace_back(conf);
	
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
	glPrograms[OVERLAY].VAO.bind();
	for (int i = 0; i < 3; i++){
		this->setAttrib(glPrograms[OVERLAY], paramNode[i], imguiVertexAttrs[i][1], imguiVertexAttrs[i][0]);
	}
#endif
	glPrograms[SPRITE2D].VAO.bind();
	glPrograms[SPRITE2D].idxType = genIdxBuffer(glPrograms[SPRITE2D].IBO, 0x3FFF);
	std::vector<uint16_t> vertices(0x10000);
	std::iota(vertices.begin(), vertices.end(), 0);
	glPrograms[TILEMAP].VAO.bind();
	glPrograms[TILEMAP].VBO.update(vertices, 0);
	glPrograms[TILEMAP].idxType = genIdxBuffer(glPrograms[TILEMAP].IBO, 0x3FFF);
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
	const auto& data = spr.render();
	if (spr.uses_stencil){
		auto& vec = m_texData[m_tex].stencilSprites;
		vec.insert(vec.end(), data.begin(), data.end());
	} else {
		auto& vec = m_texData[m_tex].sprites;
		vec.insert(vec.end(), data.begin(), data.end());
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
		currentProgram.VAO.bind();
		currentProgram.VBO = Buffer(GL_ARRAY_BUFFER);
		currentProgram.IBO = Buffer(GL_ELEMENT_ARRAY_BUFFER);
		currentProgram.VBO.bind();
		currentProgram.IBO.bind();
		currentProgram.handle.load(node[ind]);
		auto layoutNode = node[ind]["layout"];
		for (int i = 0; i < layoutNode.size(); i++){
			this->setAttrib(currentProgram, layoutNode[i]);
		}
	}
	return glPrograms.size();
}

void SpriteBatchImpl::EndFrame(const Window& target){
	target.makeCurrent();

	ConfCommand stencilOn;
	stencilOn.stencil_func = {GL_ALWAYS, 1, 255};
	stencilOn.stencil_op = {GL_KEEP, GL_KEEP, GL_REPLACE};
	stencilOn.depth_func = GL_LEQUAL;

	ConfCommand stencilOff;
	stencilOff.stencil_op = {GL_KEEP, GL_KEEP, GL_ZERO};
	
	this->m_drawlist.emplace_back(stencilOn);
	
	DrawCommand drawStencil, noStencil;
	this->drawSprites(drawStencil, true);
	this->drawSprites(noStencil, false);
	
	this->m_drawlist.emplace_back(drawStencil);
	this->m_drawlist.emplace_back(stencilOff);
	this->m_drawlist.emplace_back(noStencil);
	
	for (auto& tmap : m_Maps){
		drawTileMap(tmap, this->UBO);
	}
	
#ifndef NO_IMGUI
	ImGui::Render();
	this->drawImData(ImGui::GetDrawData());
#endif
	target.Draw(this->m_drawlist);
	target.endFrame();
	this->m_Maps.clear();
	this->m_texData.clear();
	this->m_drawlist.clear();
}
void SpriteBatchImpl::drawSprites(DrawCommand& drawComm, bool stencil){
	drawComm.program = &glPrograms[SPRITE2D].handle;
	drawComm.VAO = &glPrograms[SPRITE2D].VAO;
	int32_t baseVertex = 0;
	drawComm.bound_buffers.emplace_back(LoadCall{std::ref(glPrograms[SPRITE2D].IBO), nullptr, 0,0});
	for (auto& texturepair : m_texData){
		auto& texData = texturepair.second;
		auto* sprites = &texData.sprites;
		if (stencil){
			sprites = &texData.stencilSprites;
		}
		size_t buffersize = sprites->size()*sizeof(SpriteData);
		if (buffersize){
			DrawCall& dc = drawComm.calls.emplace_back();
			Texture& tx = dc.textures.emplace_back();
			tx.m_texture = texturepair.first;
			tx.type = GL_TEXTURE_2D;
			dc.type = Draw::Triangles;
			dc.baseVertex = baseVertex;
			dc.vtxCount = (sprites->size() / 4) * 6;
			dc.idxType = this->glPrograms[SPRITE2D].idxType;
			
			drawComm.bound_buffers.emplace_back(LoadCall{
				std::ref(glPrograms[SPRITE2D].VBO),
				sprites->data(),
				buffersize,
				baseVertex * sizeof(SpriteData)
			});

			baseVertex += sprites->size();
		}
	}
}
void SpriteBatchImpl::drawTileMap(TileMap& tilemap, Buffer& UBO){
	ConfCommand conf;
	if (tilemap.type == TMType::Normal) {
		conf.stencil_func = {GL_ALWAYS, 1, 255};
	} else if (tilemap.type == TMType::Effect) {
		conf.stencil_func = {GL_EQUAL, 1, 255};
	}

	DrawCommand dcomm;
	dcomm.VAO = &glPrograms[TILEMAP].VAO;
	dcomm.program = &glPrograms[TILEMAP].handle;

	dcomm.bound_buffers.emplace_back(LoadCall{std::ref(glPrograms[TILEMAP].VBO), nullptr, 0, 0});
	dcomm.bound_buffers.emplace_back(LoadCall{std::ref(glPrograms[TILEMAP].IBO), nullptr, 0, 0});
	dcomm.bound_buffers.emplace_back(LoadCall{std::ref(UBO), &tilemap, sizeof(UBOData), sizeof(glm::mat4)});
	dcomm.bound_buffers.emplace_back(LoadCall{std::ref(tilemap.tileTexture), tilemap.drawn.data(), sizeof(decltype(tilemap.drawn)::size_type) * tilemap.drawn.size(), 0});

	DrawCall& dc = dcomm.calls.emplace_back();
	dc.type = Draw::Triangles;
	dc.idxType = Draw::Short;
	dc.textures = {tilemap.atlasTexture, tilemap.tileBufferTBO, tilemap.tileTextureTBO};
	dc.vtxCount = tilemap.texData[0] * 6;
	dc.instances = tilemap.texData[1];

	this->m_drawlist.emplace_back(conf);
	this->m_drawlist.emplace_back(dcomm);
}
void SpriteBatchImpl::Draw(TileMap& tilemap){
	this->m_Maps.emplace_back(std::ref(tilemap));
}
#ifndef NO_IMGUI
void SpriteBatchImpl::drawImData(const ImDrawData* draw_data){
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0){
		return;
	}
	ConfCommand conf;
	conf.enable_flags[GL_BLEND] = true;
	conf.enable_flags[GL_CULL_FACE] = false;
	conf.enable_flags[GL_DEPTH_TEST] = false;
	conf.enable_flags[GL_SCISSOR_TEST] = true;
	conf.blend_equation = GL_FUNC_ADD;
	conf.blend_func = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
	m_drawlist.emplace_back(conf);
	
	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	
	const glMatrix<float, 4> mtx = {{
		{{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f }},
		{{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f }},
		{{ 0.0f,         0.0f,        -1.0f,   0.0f }},
		{{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f }},
	}};
	
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int i = 0; i < draw_data->CmdListsCount; i++){
		DrawCommand dr;
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		dr.bound_buffers.push_back({std::ref(glPrograms[OVERLAY].VBO), cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), 0});
		dr.bound_buffers.push_back({std::ref(glPrograms[OVERLAY].IBO), cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), 0});
		dr.program = &glPrograms[OVERLAY].handle;
		dr.VAO = &glPrograms[OVERLAY].VAO;
		dr.camera_override = mtx;
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++){
			DrawCall& dc = dr.calls.emplace_back();
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != nullptr){
				if (pcmd->UserCallback != ImDrawCallback_ResetRenderState){
					dc.callback = [&](const void* a, const void* b){
						const ImDrawCmd* pcd = reinterpret_cast<const ImDrawCmd*>(b);
						pcd->UserCallback(static_cast<const ImDrawList*>(a), pcd);
					};
					dc.callback_opts = {cmd_list, pcmd};
				}
			} else {
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;
				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f){
					dc.clip_rect = {
						static_cast<int>(clip_rect.x),
						static_cast<int>(fb_height - clip_rect.w),
						static_cast<int>(clip_rect.z - clip_rect.x),
						static_cast<int>(clip_rect.w - clip_rect.y)
					};
					Texture tmp;
					tmp.m_texture = reinterpret_cast<uint64_t>(pcmd->TextureId) & 0xFFFFFFFF;
					tmp.type = GL_TEXTURE_2D;
					dc.textures.emplace_back(tmp);
					dc.idxType = sizeof(ImDrawIdx) == 2 ? Draw::Short : Draw::Int;
					dc.vtxOffset = pcmd->VtxOffset;
					dc.idxOffset = (pcmd->IdxOffset * sizeof(ImDrawIdx));
					dc.vtxCount = pcmd->ElemCount;
				}
			}
		}
		m_drawlist.emplace_back(dr);
	}
	conf = {};
	conf.enable_flags[GL_DEPTH_TEST] = true;
	conf.enable_flags[GL_SCISSOR_TEST] = false;
	m_drawlist.emplace_back(conf);
}
#endif
