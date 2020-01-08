#include "gl.h"
#include <glfw3.h>
#include <iostream>
#include <zlib.h>
#include <cstring>
#include <GL/Shader.hpp>
#include <GL/Camera.hpp>
#include <GL/TextureAtlas.hpp>
#include <GL/SpriteBatch.hpp>
#include <GL/Sprite.hpp>
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>
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
int main(int, char const**) {
	float x1=0.0,x2=0.0,y1=0.0,y2=0.0;
	GLFWwindow* window;
	if (!glfwInit()){
		return 1;
	}
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(0.f,0.f,800.f,400.f));
	WindowState ws;
	bool paused = false;
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
	window = glfwCreateWindow(1280,720,"Collision Test", NULL, NULL);
	if (!window){
		glfwTerminate();
		return 2;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window,true);
	ImGui_ImplOpenGL3_Init("#version 150");
	TextureAtlas atlas("data/atlas.json");
	SpriteBatch batch(atlas, ws, "data/shaders.json");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glfwSetWindowUserPointer(window,&ws);
	glfwSetKeyCallback(window,key_callback);
	Texture t = atlas.findSubTexture("test1");
	Texture t2 = atlas.findSubTexture("test2");
	Sprite s;
	s.setTexture(t);
	Sprite s2;
	s2.setTexture(t2);
	while (!glfwWindowShouldClose(window)){
		glfwPollEvents();
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
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Collision Test");
		ImGui::SliderFloat("Object 1 x-pos",&x1,0.0f,1280.0f);
		ImGui::SliderFloat("Object 1 y-pos",&y1,0.0f,800.0f);
		ImGui::SliderFloat("Object 2 x-pos",&x2,0.0f,1280.0f);
		ImGui::SliderFloat("Object 2 y-pos",&y2,0.0f,800.0f);
		ImGui::Text("%s",colstr.c_str());
		ImGui::End();
		ImGui::Render();

		glfwMakeContextCurrent(window);
		glUniformMatrix4fv(ws.MatrixID,1,GL_FALSE,&ws.camera->getVP()[0][0]);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		batch.Draw(&s);
		batch.Draw(&s2);
		batch.Draw(window);
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
