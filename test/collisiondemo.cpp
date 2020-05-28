#include <iostream>
#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Camera.hpp>
#include <GL/Window.hpp>
#include <GL/Sprite.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
int main(int, char const**) {
	float x1=0.0,x2=0.0,y1=0.0,y2=0.0;
	Window window(1280, 720, 3, 3, "resources/data/fonts/boxfont_round.ttf", "Collision Test");
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(0.f,0.f,800.f,400.f), window);
	bool paused = false;
	window.getWindowState().camera = &camera;
	TextureAtlas atlas("resources/data/atlas.json");
	SpriteBatch batch(atlas, "resources/data/shaders.json");
	Texture t = atlas.findSubTexture("test1");
	Texture t2 = atlas.findSubTexture("test2");
	Sprite s;
	s.setTexture(t);
	Sprite s2;
	s2.setTexture(t2);
	while (window.isOpen()){
		WindowState& ws = window.getWindowState();
		s.setPosition(x1,y1);
		s2.setPosition(x2,y2);
		if (ws.keyboardState[GLFW_KEY_A]){
			paused = true;
			ws.keyboardState[GLFW_KEY_A] = false;
		}
		if (!paused){
			s2.rotate(-0.1f);
			s.rotate(0.1f);
		}
		bool collided = s.PPCollidesWith(s2);
		std::string colstr = "no";
		if (collided){
			colstr = "yes";
		}
		window.startFrame();
		window.makeCurrent();
		ImGui::Begin("Collision Test");
		ImGui::SliderFloat("Object 1 x-pos",&x1,0.0f,1280.0f);
		ImGui::SliderFloat("Object 1 y-pos",&y1,0.0f,800.0f);
		ImGui::SliderFloat("Object 2 x-pos",&x2,0.0f,1280.0f);
		ImGui::SliderFloat("Object 2 y-pos",&y2,0.0f,800.0f);
		ImGui::Text("%s",colstr.c_str());
		ImGui::End();

		batch.Draw(s);
		batch.Draw(s2);
		batch.Draw(window);
		window.endFrame(&batch);
	}
	return 0;
}
