#include "gl.h"
#include <iostream>
#include <zlib.h>
#include "game/Character.hpp"
#include "game/Enemy.hpp"
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
#include "GL/TextureAtlas.hpp"
#include "GL/SpriteBatch.hpp"
#include "GL/WindowState.h"
#include "GL/Window.hpp"
#include "core/Map.hpp"
int main(){
	const float frametime = 1.f/60.f;
	Window window(1280, 720, 3, 3, "data/fonts/boxfont_round.ttf", "Starry Sky");
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(200.f,200.f,800.f,400.f));
	WindowState ws;
	ws.camera = &camera;
	window.setWindowState(&ws);
	glm::mat4 m(1.0f);
	TextureAtlas atlas("data/atlas.json");
	ObjMap map("map.json",atlas);
	Character player(300, 300, map, "lesserdog", "sword2", "sword", atlas);
	Enemy testEnemy(900, 300, map, "birdo1", "birdo3", 10, atlas);
	std::vector<MovingEntity*> objects;
	objects.push_back(&testEnemy);
	SpriteBatch batch(atlas, ws, "data/shaders.json");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	while (window.isOpen()){
		window.startFrame();
		window.makeCurrent();
		map.Draw(batch);
		player.Update(frametime,objects,window);
		testEnemy.Update(frametime,&player);
		player.Draw(batch);
		testEnemy.Draw(batch);
		if (player.dead){
			player.dead = false;
			player.warpto(300.f,300.f);
			testEnemy.warpto(900.f,300.f);
			testEnemy.reset();
			camera.reset();
		}else if (testEnemy.dead){
			objects.clear();
			std::cout << "You Win" << std::endl;
			window.close();
		}
		batch.Draw(window);
		window.endFrame();
	}
	return 0;
}