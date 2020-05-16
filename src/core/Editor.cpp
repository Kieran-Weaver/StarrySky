#include <core/Editor.hpp>
#include <GL/Tilemap.hpp>
#include <util/Mat2D.hpp>
#include <imgui/imgui.h>
#include <vector>
static bool map_changed = false;
bool Mat2GUI(glm::mat2& mdata, const std::string& tag, const float& max_value){
	constexpr double m_pi = std::atan(1.f)*4.f;
	std::array<float,5> matrix_data = decomp(mdata);

	ImGui::Begin(("##" + tag).c_str());
	bool changed = ImGui::SliderFloat4(("Matrix Coefficients##" + tag).c_str(),&mdata[0][0], 0.f, max_value);
	if (changed){
		matrix_data = decomp(mdata);
	}
	std::array<float, 5> temp{matrix_data};
	if (ImGui::SliderFloat(("Rotation##" + tag).c_str(), &(temp[0]), -m_pi, m_pi)){
		matrix_data[0] = temp[0];
		mdata = recompose(matrix_data);
	}
	ImGui::SliderFloat2(("Scale##" + tag).c_str(), &(temp[1]), -max_value, max_value);
	ImGui::SliderFloat2(("Shear##" + tag).c_str(), &(temp[3]), -max_value, max_value);
	ImGui::End();
	return changed;
}
LevelEditor::LevelEditor(ObjMap& map) : map(map){}
void LevelEditor::Draw(SpriteBatch& batch){
	sprites.clear();
	for (auto& rect : rects){
		sprites.emplace_back();
		sprites.back().setTexture(selectedTex);
		sprites.back().transform(ScaleMat(1.f/selectedTex.m_rect.width, 1.f/selectedTex.m_rect.height));
		sprites.back().transform(ScaleMat(rect.width, rect.height));
	}
	rects.clear();
	for (auto& sprite : sprites){
		batch.Draw(sprite);
	}
	TileMap& temp_tm = map.getTM("Editor");
	temp_tm.drawn = tiles;
	tiles.clear();
}
