#include "Editor.hpp"
#include <imgui/imgui.h>
#include <GL/Mat2D.hpp>
bool Mat2GUI(glm::mat2& mdata, const std::string& tag, const float& max_value){
	constexpr double m_pi = std::atan(1.f)*4.f;
	std::array<float,5> matrix_data = decomp(mdata);

	ImGui::Begin(("##" + tag).c_str());
	bool changed = ImGui::SliderFloat4(("Matrix Coefficients##" + tag).c_str(),&mdata[0][0], 0.f, max_value);
	if (changed){
		matrix_data = decomp(mdata);
	}
	std::array<float, 5> temp{matrix_data};
	ImGui::SliderFloat(("Rotation##" + tag).c_str(), &(temp[0]), -m_pi, m_pi);
	ImGui::SliderFloat2(("Scale##" + tag).c_str(), &(temp[1]), -max_value, max_value);
	ImGui::SliderFloat2(("Shear##" + tag).c_str(), &(temp[3]), -max_value, max_value
	);
	ImGui::End();
	matrix_data[0] = temp[0];
	mdata = recompose(matrix_data);
	return changed;
}