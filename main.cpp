#include "gl.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <zlib.h>
#include <cstring>
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
#include "GL/TextureAtlas.h"
#include "GL/SpriteBatch.h"
#include "GL/WindowState.h"
#include "core/Map.hpp"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GLFW_TRUE);
	}
	WindowState * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS){
		if (key == GLFW_KEY_LEFT){
			ws->camera->Scroll(glm::vec2(-50.f,0.f));
		}else if (key == GLFW_KEY_RIGHT){
			ws->camera->Scroll(glm::vec2(50.f,0.f));
		}else if (key == GLFW_KEY_UP){
			ws->camera->Scroll(glm::vec2(0.f,50.f));
		}else if (key == GLFW_KEY_DOWN){
			ws->camera->Scroll(glm::vec2(0.f,-50.f));
		}
	}
	ImGui_ImplGlfw_KeyCallback(window,key,scancode,action,mods);
}
int main(void){
	GLFWwindow* window;
	if (!glfwInit()){
		return 1;
	}
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(0.f,0.f,800.f,400.f));
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
	window = glfwCreateWindow(1280,720,"Starry Sky", NULL, NULL);
	if (!window){
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
	atlas.loadFromFile("data/atlas.bin");
	ObjMap map("map2.txt",atlas);
	Texture t = atlas.findSubTexture("bookshelf");
	Texture t2 = atlas.findSubTexture("shield");
	Sprite s(t);
	Sprite s2(t2);
	SpriteBatch batch(atlas,ws);
	glfwSetWindowUserPointer(window,&ws);
	glfwSetKeyCallback(window,key_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	while (!glfwWindowShouldClose(window)){
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();
		glfwMakeContextCurrent(window);
		glUniformMatrix4fv(ws.MatrixID,1,GL_FALSE,&ws.camera->getVP()[0][0]);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		map.Draw(batch);
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
