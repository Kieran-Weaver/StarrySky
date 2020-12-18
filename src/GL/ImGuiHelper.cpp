#include <GL/ImGuiHelper.hpp>
#ifndef NO_IMGUI
#include <imgui/imgui.h>
#include <string>
void ImImage(const Texture& tex, float width, float height){
	float texX1 = tex.m_rect.left / 65536.f;
	float texX2 = tex.m_rect.right / 65536.f;
	float texY1 = tex.m_rect.top / 65536.f;
	float texY2 = tex.m_rect.bottom / 65536.f;
	ImGui::Image((void*)(intptr_t)tex.m_texture, ImVec2(width, height), ImVec2(texX1, texY1), ImVec2(texX2, texY2));
}
bool ImImageButton(const std::string& id, const Texture& tex, float width, float height){
	float texX1 = tex.m_rect.left / 65536.f;
	float texX2 = tex.m_rect.right / 65536.f;
	float texY1 = tex.m_rect.top / 65536.f;
	float texY2 = tex.m_rect.bottom / 65536.f;
	ImGui::PushID((std::string("##") + id).c_str());
	bool pressed = ImGui::ImageButton((void*)(intptr_t)tex.m_texture, ImVec2(width, height), ImVec2(texX1, texY1), ImVec2(texX2, texY2));
	ImGui::PopID();
	return pressed;
}
#endif
