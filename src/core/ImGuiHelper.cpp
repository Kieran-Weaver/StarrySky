#include <core/ImGuiHelper.hpp>
#include <imgui/imgui.h>
void SliderTLeftLabel(const char* label, int* tpointer, int lower_bound, int upper_bound){
	ImGui::Text("%s",label);
	ImGui::SameLine();
	ImGui::SliderInt(label,tpointer,lower_bound,upper_bound);
}
void SliderTLeftLabel(const char* label, float* tpointer, float lower_bound, float upper_bound){
	ImGui::Text("%s",label);
	ImGui::SameLine();
	ImGui::SliderFloat(label,tpointer,lower_bound,upper_bound);
}
