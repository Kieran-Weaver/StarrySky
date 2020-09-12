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
		currentmap["default"] = TileMap();
		mapIt = currentmap.begin();
	}
	~EditorState(){
		delete this->window.getWindowState().camera;
	}
	TileMap& getMap(){
		return mapIt->second;
	}
	Window window;
	SpriteBatch batch;
	TextureAtlas atlas;
	std::map<std::string,TileMap> currentmap;
	std::map<std::string,Texture> textures;
	std::map<std::string,TileMap>::iterator mapIt;
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
	return st->textures[st->getMap().filenames[i]];
}
glm::ivec2 getMouseTile(EditorState* st){
	const std::array<float, 4>& affT = st->getMap().AffineT;
	const std::array<float, 4>& mAttrs = st->getMap().Attrs;
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


	state->mapData.AffineT = state->getMap().AffineT;
	state->mapData.Attrs = state->getMap().Attrs;
	state->mapData.metadata = state->getMap().metadata;
	state->mapW = state->getMap().texData[0];
	state->mapH = state->getMap().texData[1];
	
	auto texIt = state->textures.begin();
	int editorMode = 0;
	char filenamebuf[256] = {};
	char namebuf[256] = {};
	while (state->window.isOpen()){
		state->window.startFrame();
		state->window.makeCurrent();
		glm::vec2 pos = getMouseTile(state);
		if (ImGui::Begin("Tilemap")){
			ImGui::TextUnformatted("Tilemap Name:");
			ImGui::TextUnformatted(state->mapIt->first.c_str());
			ImGui::TextUnformatted("Tile Textures:");
			ImGuiStyle& style = ImGui::GetStyle();
			float visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (int i = 0; i < state->getMap().filenames.size(); i++){
				if (ImImageButton(state->getMap().filenames[i], getTile(state, i), 80, 80)){
					state->currTex = i;
					texIt = state->textures.find(state->getMap().filenames[i]);
				}
				float last_x2 = ImGui::GetItemRectMax().x;
				float next_x2 = last_x2 + style.ItemSpacing.x + 80;
				if ((next_x2 < visible_x2) && (i != (state->getMap().filenames.size() - 1))){
					ImGui::SameLine();
				}
			}
			ImGui::End();
		} else {
			ImGui::End();
		}
		if (ImGui::Begin("Editor", NULL, ImGuiWindowFlags_MenuBar)){
			if (ImGui::BeginMenuBar()){
				if (ImGui::MenuItem("New##Layer")){
					editorMode = 0;
				}
				if (ImGui::MenuItem("Save##to File")){
					editorMode = 1;
				}
				if (ImGui::MenuItem("Load##from File")){
					editorMode = 2;
				}
				if (ImGui::MenuItem("Metadata")){
					editorMode = 3;
				}
				if (ImGui::MenuItem("Textures")){
					editorMode = 4;
				}
				if (ImGui::MenuItem("Display All")){
					editorMode = 5;
				}
				if (ImGui::MenuItem("Switch Maps")){
					state->mapIt++;
					if (state->mapIt == state->currentmap.end()){
						state->mapIt = state->currentmap.begin();
					}
				}
				ImGui::EndMenuBar();
			}
			if (editorMode == 0) {
				ImGui::InputText("Tilemap Name", namebuf, 256);
				std::string nme(namebuf);
				if (ImGui::Button("New")){
					if (nme.find_first_not_of(' ') != std::string::npos){
						state->currentmap[nme] = TileMap();
						state->mapIt = state->currentmap.find(nme);
					}
				}
				if (ImGui::Button("Delete") && (state->currentmap.size() > 1)){
					auto mIt = state->currentmap.find(nme);
					if (mIt != state->currentmap.end()){
						state->currentmap.erase(mIt);
					}
				}
			} else if (editorMode == 1) {
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
						for (const auto& cmap : state->currentmap){
							writer.Key(cmap.first);
							writer.store(cmap.second);
						}
						writer.EndObject();
						ofs << writer.GetString() << std::endl;
						ofs.close();
					}
				}
			} else if (editorMode == 2){
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
						state->currentmap[key] = TileMap(node[key], state->atlas);
						state->mapIt = state->currentmap.find(key);
						state->mapData.AffineT = state->getMap().AffineT;
						state->mapData.Attrs = state->getMap().Attrs;
						state->mapData.metadata = state->getMap().metadata;
						state->mapW = state->getMap().texData[0];
						state->mapH = state->getMap().texData[1];
					}
				}
			}  else if (editorMode == 3) {
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
					state->getMap().AffineT = state->mapData.AffineT;
					state->getMap().Attrs = state->mapData.Attrs;
					state->getMap().metadata = state->mapData.metadata;
					state->getMap().texData[0] = state->mapW;
					state->getMap().texData[1] = state->mapH;
					state->getMap().drawn.resize(state->mapW * state->mapH);
				}
			} else if (editorMode == 4) {
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
					if (std::find(state->getMap().filenames.begin(), state->getMap().filenames.end(), texIt->first) == state->getMap().filenames.end()){
						state->getMap().addTile(texIt->first, texIt->second);
						state->getMap().loadTiles();
					}
				}
				ImGui::Text("Mouse position: %f, %f", state->mouseX, state->mouseY);
				ImGui::Text("Mouse tile: %d, %d", (int)pos[0], (int)pos[1]);
			} else if (editorMode == 5) {
				ImGui::TextUnformatted("Map Names: ");
				for (auto& cmap : state->currentmap){
					ImGui::TextUnformatted(cmap.first.c_str());
					state->batch.Draw(cmap.second);
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
					state->getMap().drawn[mousePos] = state->currTex;
				}
			}
		}
		state->batch.Draw(state->getMap());
		state->batch.EndFrame(state->window);
	}
	delete state;
	return 0;
}
