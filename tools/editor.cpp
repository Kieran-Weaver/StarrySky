#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Window.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <util/Mat2D.hpp>
#include <GL/Tilemap.hpp>
#include <imgui/imgui.h>
#include <GL/ImGuiHelper.hpp>
#include <map>
struct EditorState{
	EditorState() :
	window(1280, 720, 3, 3, "resources/data/fonts/boxfont_round.ttf", "Starry Sky Editor", false),
	atlas("resources/data/atlas.json"),
	batch(this->atlas, "resources/data/shaders.json")
	{
		this->window.getWindowState().camera = new Camera(Rect<float>(0.f,0.f,1400.f,800.f),Rect<float>(300.f,200.f,600.f,400.f), this->window);
		for (auto& tex_name : atlas.getSubTextureNames()){
			this->currentmap.filenames.emplace_back(tex_name);
			textures[tex_name] = atlas.findSubTexture(tex_name);
		}
		std::sort(this->currentmap.filenames.begin(), this->currentmap.filenames.end());
	}
	~EditorState(){
		delete this->window.getWindowState().camera;
	}
	Window window;
	SpriteBatch batch;
	TextureAtlas atlas;
	TileMap currentmap;
	std::map<std::string,Texture> textures;
	int currTex = 0;
	double mouseX = 0.0;
	double mouseY = 0.0;
	bool shifted = false;
	bool scrollkey = false;
	int mapW = 0;
	int mapH = 0;
	UBOData mapData;
};
EditorState* state = nullptr;
void drawRect(int x0, int y0, int x1, int y1, unsigned int color){
	Sprite blanksp;
	blanksp.setTexture(state->textures["blank"]);
	double w = (x1-x0);
	double h = (y1-y0);
	std::array<uint8_t, 4> col;
	col[0] = color & 0xFF000000 >> 24;
	col[1] = color & 0x00FF0000 >> 16;
	col[2] = color & 0x0000FF00 >> 8;
	col[3] = color & 0x000000FF;
	blanksp.transform(ScaleMat(w / state->textures["blank"].width, h / state->textures["blank"].height));
	blanksp.setPosition(x0 + (w/2), y0 + (h/2));
	blanksp.setColor(col);
	state->batch.Draw(blanksp);
}
void drawTile(int x0, int y0, unsigned short id){
//	Texture stx 
	Sprite sp;
	sp.setTexture(state->textures[state->currentmap.filenames[id]]);
	sp.transform(ScaleMat(32.0 / sp.m_subtexture.width, 32.0 / sp.m_subtexture.height));
	sp.setPosition(x0 + 16, y0 + 16);
	state->batch.Draw(sp);
}
void MoveCB(double xpos, double ypos){
	if (state){
		state->mouseX = xpos;
		state->mouseY = ypos;
	}
}
void ButtonCB(int button, int action, int mods){
	if (state){
		state->shifted = mods & 1;
		state->scrollkey = mods & 2;
	}
}
int main(){
	state = new EditorState();
	state->window.getWindowState().cursorCB = MoveCB;
	state->window.getWindowState().mouseCB = ButtonCB;

	state->currentmap.AffineT = { 1.f, 0.f, 0.f, 1.f };
	state->currentmap.Attrs = { 150.f, 150.f, 75.f, 75.f };
	state->currentmap.texData = { 4, 1, 0 , 0 };
	state->currentmap.addTile("cubby0", state->atlas.findSubTexture("cubby0"));
	state->currentmap.drawn = { 0, 0, 0, 0 };
	state->currentmap.loadTiles();

	state->mapData.AffineT = state->currentmap.AffineT;
	state->mapData.Attrs = state->currentmap.Attrs;
	state->mapData.metadata = state->currentmap.metadata;
	state->mapW = 4;
	state->mapH = 1;
	
	auto texIt = state->textures.begin();
	const std::array<float, 4>& affT = state->currentmap.AffineT;
	const std::array<float, 4>& mAttrs = state->currentmap.Attrs;
	glm::mat2 unmat = glm::inverse(glm::mat2(affT[0],affT[1],affT[2],affT[3]));
	int editorMode = 0;
	while (state->window.isOpen()){
		state->window.startFrame();
		state->window.makeCurrent();
		if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar)){
			if (ImGui::BeginMenuBar()){
				if (ImGui::MenuItem("View Textures")){
					editorMode = 0;
				}
				if (ImGui::MenuItem("Edit Map Metadata")){
					editorMode = 1;
				}
				ImGui::EndMenuBar();
			}
			if (editorMode == 0) {
				ImGui::TextUnformatted(texIt->first.c_str());
				ImImage(texIt->second, 160, 160);
				if (ImGui::Button("Previous")){
					if (texIt != state->textures.begin()){
						texIt--;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Next")){
					if (++texIt == state->textures.end()){
						texIt--;
					}
				}
				ImGui::Text("Mouse position: %f, %f", state->mouseX, state->mouseY);
				glm::vec2 pos = unmat * glm::vec2((state->mouseX - mAttrs[2]) / mAttrs[0] + 0.5f, (state->mouseY - mAttrs[3]) / mAttrs[1] + 0.5f);
				ImGui::Text("Mouse tile: %d, %d", (int)pos[0], (int)pos[1]);
			} else if (editorMode == 1) {
				ImGui::TextUnformatted("Affine Transformation: ");
				ImGui::DragFloat2("##AffineT1", &(state->mapData.AffineT[0]), 0.1f);
				ImGui::DragFloat2("##AffineT3", &(state->mapData.AffineT[2]), 0.1f);
				ImGui::TextUnformatted("Tile Dimensions: ");
				ImGui::DragFloat("Width##TileDM1", &(state->mapData.Attrs[0]), 1.0f);
				ImGui::DragFloat("Height##TileDM2", &(state->mapData.Attrs[1]), 1.0f);
				ImGui::TextUnformatted("Tilemap Offset: ");
				ImGui::DragFloat("X Position##MapOff1", &(state->mapData.Attrs[2]), 1.0f);
				ImGui::DragFloat("Y Position##MapOff2", &(state->mapData.Attrs[3]), 1.0f);
				ImGui::DragFloat("Z Position##MapOff3", &(state->mapData.metadata[0]), 0.1f);
				ImGui::TextUnformatted("Tilemap Size: ");
				ImGui::DragInt("Width##MapSize", &(state->mapW), 0.1f);
				ImGui::DragInt("Height##MapSize", &(state->mapH), 0.1f);
				if (ImGui::Button("Save")){
					state->currentmap.AffineT = state->mapData.AffineT;
					state->currentmap.Attrs = state->mapData.Attrs;
					state->currentmap.metadata = state->mapData.metadata;
					state->currentmap.texData[0] = state->mapW;
					state->currentmap.texData[1] = state->mapH;
				}
			}
			ImGui::End();
		} else {
			ImGui::End();
		}
		state->batch.Draw(state->currentmap);
		state->batch.EndFrame(state->window);
	}
	delete state;
	return 0;
}
