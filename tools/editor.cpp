#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Window.hpp>
#include <GL/Camera.hpp>
#include <GL/Sprite.hpp>
#include <util/Mat2D.hpp>
#include <GL/Tilemap.hpp>
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
	Texture allWhite;
	TileMap currentmap;
	std::unordered_map<std::string,Texture> textures;
	double mouseX = 0.0;
	double mouseY = 0.0;
	bool shifted = false;
	bool scrollkey = false;
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
	state->currentmap.Attrs = { 150.f, 150.f, 0, 0 };
	state->currentmap.texData = { 4, 1, 0 , 0 };
	state->currentmap.filenames = { "bs1" };
	state->currentmap.drawn = { 0, 0, 0, 0, 0 };
	for (auto& tfile : state->currentmap.filenames){
		const Texture tempTex = state->atlas.findSubTexture(tfile);
		state->currentmap.atlasTexture.m_texture = tempTex.m_texture;
		state->currentmap.tileData.emplace_back();
		state->currentmap.tileData.back()[0] = tempTex.m_rect.left / 65536.f;
		state->currentmap.tileData.back()[1] = tempTex.m_rect.top / 65536.f;
		state->currentmap.tileData.back()[2] = tempTex.m_rect.width / 65536.f;
		state->currentmap.tileData.back()[3] = tempTex.m_rect.height / 65536.f;
		state->currentmap.numTiles++;
	}
	state->currentmap.loadTiles();
	while (state->window.isOpen()){
		state->window.startFrame();
		state->window.makeCurrent();
		state->batch.Draw(state->currentmap);
		state->batch.EndFrame(state->window);
	}
	delete state;
	return 0;
}
