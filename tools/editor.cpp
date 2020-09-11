#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Window.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <GL/Tilemap.hpp>
#include <GL/ImGuiHelper.hpp>
#include <util/Mat2D.hpp>
#include <imgui/imgui.h>
#include <file/PlainText.hpp>
#include <file/JSON.hpp>
#include <map>
#include <fstream>
struct EditorState{
	EditorState() :
	window(1280, 720, 3, 3, "resources/data/fonts/boxfont_round.ttf", "Starry Sky Editor", false),
	atlas("resources/data/atlas.json"),
	batch(this->atlas, "resources/data/shaders.json")
	{
		this->window.getWindowState().camera = new Camera(Rect<float>(0.f,0.f,1400.f,800.f),Rect<float>(300.f,200.f,600.f,400.f), this->window);
		for (auto& tex_name : atlas.getSubTextureNames()){
			textures[tex_name] = atlas.findSubTexture(tex_name);
		}
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
	bool mouseDown = false;
	int mapW = 0;
	int mapH = 0;
	UBOData mapData;
};
EditorState* state = nullptr;
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
		if (action){
			state->mouseDown = true;
		} else {
			state->mouseDown = false;
		}
	}
}
const Texture& getTile(EditorState* st, int i){
	return st->textures[st->currentmap.filenames[i]];
}
glm::ivec2 getMouseTile(EditorState* st){
	const std::array<float, 4>& affT = st->currentmap.AffineT;
	const std::array<float, 4>& mAttrs = st->currentmap.Attrs;
	glm::mat2 unmat = glm::inverse(glm::mat2(affT[0],affT[1],affT[2],affT[3]));
	return unmat * glm::vec2((state->mouseX - mAttrs[2]) / mAttrs[0] + 0.5f, (state->mouseY - mAttrs[3]) / mAttrs[1] + 0.5f);
}
template<>
void JSONWriter::store<TMType>(const TMType& i){
	if (i == TMType::Normal){
		writer.String("normal");
	} else if (i == TMType::Effect){
		writer.String("effect");
	}
}
int main(){
	state = new EditorState();
	state->window.getWindowState().cursorCB = MoveCB;
	state->window.getWindowState().mouseCB = ButtonCB;

	state->currentmap.AffineT = { 1.f, 0.f, 0.f, 1.f };
	state->currentmap.Attrs = { 150.f, 150.f, 75.f, 75.f };
	state->currentmap.texData = { 4, 1, 0 , 0 };
	state->currentmap.addTile("cubby0", state->textures["cubby0"]);
	state->currentmap.drawn = { 1, 1, 1, 1 };
	state->currentmap.loadTiles();

	state->mapData.AffineT = state->currentmap.AffineT;
	state->mapData.Attrs = state->currentmap.Attrs;
	state->mapData.metadata = state->currentmap.metadata;
	state->mapW = state->currentmap.texData[0];
	state->mapH = state->currentmap.texData[1];
	
	auto texIt = state->textures.begin();
	int editorMode = 0;
	char filenamebuf[256] = {};
	char namebuf[256] = {};
	while (state->window.isOpen()){
		state->window.startFrame();
		state->window.makeCurrent();
		glm::vec2 pos = getMouseTile(state);
		if (ImGui::Begin("Tilemap")){
			ImGui::TextUnformatted("Tile Textures:");
			for (int i = 0; i < state->currentmap.filenames.size(); i++){
				if (ImImageButton(state->currentmap.filenames[i], getTile(state, i), 80, 80)){
					state->currTex = i;
					texIt = state->textures.find(state->currentmap.filenames[i]);
				}
				if ((i % 2 == 0) && (i != (state->currentmap.filenames.size() - 1))){
					ImGui::SameLine();
				}
			}
			ImGui::End();
		} else {
			ImGui::End();
		}
		if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar)){
			if (ImGui::BeginMenuBar()){
				if (ImGui::MenuItem("View Textures")){
					editorMode = 0;
				}
				if (ImGui::MenuItem("Edit Map Metadata")){
					editorMode = 1;
				}
				if (ImGui::MenuItem("Save##to File")){
					editorMode = 2;
				}
				if (ImGui::MenuItem("Load##from File")){
					editorMode = 3;
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
				if (ImGui::Button("Add to Tilemap")){
					if (std::find(state->currentmap.filenames.begin(), state->currentmap.filenames.end(), texIt->first) == state->currentmap.filenames.end()){
						state->currentmap.addTile(texIt->first, texIt->second);
						state->currentmap.loadTiles();
					}
				}
				ImGui::Text("Mouse position: %f, %f", state->mouseX, state->mouseY);
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
					state->currentmap.drawn.resize(state->mapW * state->mapH);
				}
			} else if (editorMode == 2) {
				ImGui::InputText("Filename", filenamebuf, 256);
				std::ifstream ifs(filenamebuf);
				if (ifs.good()){
					ImGui::TextUnformatted("Error: File exists");
				}
				if (ImGui::Button("Save")){
					if (!ifs.good()){
						std::ofstream ofs(filenamebuf);
						JSONWriter writer;
						writer.StartObject();
						writer.Key("tm");
						writer.store(state->currentmap);
						writer.EndObject();
						ofs << writer.GetString() << std::endl;
						ofs.close();
					}
				}
			} else if (editorMode == 3){
				ImGui::InputText("Filename", filenamebuf, 256);
				ImGui::InputText("Tilemap Name", namebuf, 256);
				std::ifstream ifs(filenamebuf);
				if (!ifs.good()){
					ImGui::TextUnformatted("Error: File does not exist");
				}
				if (ImGui::Button("Load")){
					std::string jsondata = readWholeFile(filenamebuf);
					JSONReader document(jsondata.c_str());
					auto node = document["tilemaps"];
					std::string key(namebuf);
					if (node.HasKey(key)){
						state->currentmap = TileMap(node[key], state->atlas);
						state->mapData.AffineT = state->currentmap.AffineT;
						state->mapData.Attrs = state->currentmap.Attrs;
						state->mapData.metadata = state->currentmap.metadata;
						state->mapW = state->currentmap.texData[0];
						state->mapH = state->currentmap.texData[1];
					}
				}
			}
			ImGui::End();
		} else {
			ImGui::End();
		}
		if (!(ImGui::GetIO().WantCaptureMouse)){
			if (state->mouseDown){
				int mousePos = pos[1] * state->mapW + pos[0];
				if ((pos[0] < state->mapW) && (pos[0] >= 0) && (pos[1] >= 0) && (pos[1] < state->mapH)){
					state->currentmap.drawn[mousePos] = state->currTex;
				}
			}
		}
		state->batch.Draw(state->currentmap);
		state->batch.EndFrame(state->window);
	}
	delete state;
	return 0;
}
