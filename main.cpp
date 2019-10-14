#include "gl.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <zlib.h>
#include "game/Character.hpp"
#include "game/Enemy.hpp"
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
#include "GL/TextureAtlas.hpp"
#include "GL/SpriteBatch.hpp"
#include "GL/WindowState.h"
#include "core/Map.hpp"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GLFW_TRUE);
	}
	auto * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS){
		ws->keyboardState[key] = true;
	}else if (action == GLFW_RELEASE){
		ws->keyboardState[key] = false;
	}
	ImGui_ImplGlfw_KeyCallback(window,key,scancode,action,mods);
}
int main(){
	const float frametime = 1.f/60.f;
	GLFWwindow* window = nullptr;
	if (glfwInit() != GLFW_TRUE){
		return 1;
	}
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(200.f,200.f,800.f,400.f));
	WindowState ws;
	ws.camera = &camera;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	window = glfwCreateWindow(1280,720,"Starry Sky", nullptr, nullptr);
	if (window == nullptr){
		glfwTerminate();
		return 2;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window,true);
	ImGui_ImplOpenGL3_Init("#version 150");
	io.Fonts->AddFontFromFileTTF("data/fonts/boxfont_round.ttf",20.f);
	glm::mat4 m(1.0f);
	TextureAtlas atlas;
	if (!atlas.loadFromFile("data/atlas.json")){
		std::cout << "Could not load atlas from data/atlas.json" << std::endl;
		return 3;
	}
	ObjMap map("map2.txt",atlas);
	Character player(300, 300, map, "lesserdog", "sword2", "sword", atlas);
	Enemy testEnemy(900, 300, map, "birdo1", "birdo3", 10, atlas);
	std::vector<MovingEntity*> objects;
	objects.push_back(&testEnemy);
	SpriteBatch batch(atlas,ws);
	glfwSetWindowUserPointer(window,&ws);
	glfwSetKeyCallback(window,key_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	while (glfwWindowShouldClose(window) == GLFW_FALSE){
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glfwMakeContextCurrent(window);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
			glfwSetWindowShouldClose(window,GLFW_TRUE);
		}
		batch.Draw(window);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
