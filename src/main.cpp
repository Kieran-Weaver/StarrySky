#include <game/Character.hpp>
#include <game/Enemy.hpp>
#include <GL/Camera.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Window.hpp>
#include <core/Map.hpp>
int main(){
	const float frametime = 1.f/60.f;
	Window window(1280, 720, 3, 3, "resources/data/fonts/boxfont_round.ttf", "Starry Sky", false);
	Camera camera({0.f,0.f,1400.f,800.f},{300.f,200.f,600.f,400.f}, window);
	window.getWindowState().camera = &camera;
	glm::mat4 m(1.0f);
	TextureAtlas atlas("resources/data/atlas.json");
	ObjMap map("resources/map.json",atlas);
	Character player(300, 300, map, "player", "sword1", "sword2", atlas);
	Enemy testEnemy(900, 300, map, "robot", "robot", 10, atlas);
	std::vector<MovingEntity*> objects;
	objects.push_back(&testEnemy);
	SpriteBatch batch(atlas, "resources/data/shaders.json");
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
			window.close();
		}
		batch.EndFrame(window);
	}
	return 0;
}
