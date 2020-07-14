#include <GL/DrawCommand.hpp>
#include <imgui/imgui.h>
#include <gl.h>
DrawList toDrawList(const ImDrawData* draw_data, Program& program, Buffer& vtxBuf, Buffer& idxBuf, VertexArray& VAO){
	int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0){
		return {};
	}
	DrawList dlist = {};
	ConfCommand conf;
	conf.enable_flags[GL_BLEND] = true;
	conf.enable_flags[GL_CULL_FACE] = false;
	conf.enable_flags[GL_DEPTH_TEST] = false;
	conf.enable_flags[GL_SCISSOR_TEST] = true;
	conf.blend_equation = GL_FUNC_ADD;
	conf.blend_func = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
	dlist.emplace_back(conf);
	
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
		dr.bound_buffers[GL_ARRAY_BUFFER] = {};
		dr.bound_buffers[GL_ARRAY_BUFFER].buff = &vtxBuf;
		dr.bound_buffers[GL_ARRAY_BUFFER].data = cmd_list->VtxBuffer.Data;
		dr.bound_buffers[GL_ARRAY_BUFFER].size = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
		dr.bound_buffers[GL_ELEMENT_ARRAY_BUFFER] = {};
		dr.bound_buffers[GL_ELEMENT_ARRAY_BUFFER].buff = &idxBuf;
		dr.bound_buffers[GL_ELEMENT_ARRAY_BUFFER].data = cmd_list->IdxBuffer.Data;
		dr.bound_buffers[GL_ELEMENT_ARRAY_BUFFER].size = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
		dr.program = &program;
		dr.VAO = &VAO;
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
					dc.textures.emplace_back(reinterpret_cast<const uint64_t>(pcmd->TextureId));
					dc.idxType = sizeof(ImDrawIdx) == 2 ? Draw::Short : Draw::Int;
					dc.vtxOffset = pcmd->VtxOffset;
					dc.idxOffset = (pcmd->IdxOffset * sizeof(ImDrawIdx));
					dc.vtxCount = pcmd->ElemCount;
				}
			}
		}
		dlist.emplace_back(dr);
	}
	return dlist;
}
