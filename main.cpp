#include "gl.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <zlib.h>
#include <cstring>
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
#include "GL/TextureAtlas.h"
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GLFW_TRUE);
	}
	Camera * c = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS){
		if (key == GLFW_KEY_LEFT){
			c->Scroll(glm::vec2(-50.f,0.f));
		}else if (key == GLFW_KEY_RIGHT){
			c->Scroll(glm::vec2(50.f,0.f));
		}else if (key == GLFW_KEY_UP){
			c->Scroll(glm::vec2(0.f,50.f));
		}else if (key == GLFW_KEY_DOWN){
			c->Scroll(glm::vec2(0.f,-50.f));
		}
	}
}
int main(void){
	GLFWwindow* window;
	if (!glfwInit()){
		return 1;
	}
	Camera camera(Rect<float>(-2000.f,-2000.f,4000.f,4000.f),Rect<float>(0.f,0.f,800.f,400.f));
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
	glfwSetWindowUserPointer(window,&camera);
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window,true);
	ImGui_ImplOpenGL3_Init("#version 150");
	io.Fonts->AddFontFromFileTTF("data/fonts/boxfont_round.ttf",20.f);
	float vertices[] = {
		 0.f,0.f,0.0f,0.0f,
		 2000.f,0.f,1.0f,0.0f,
		 2000.f,1000.f,1.0f,1.0f,
		 0.f,1000.f,0.0f,1.0f
	};
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	GLBuffer<uint8_t> ebo = genIndexBuffer<uint8_t>(250);

	Shader vxShader(GL_VERTEX_SHADER);
	vxShader.load("data/shaders/default.vert");

	Shader fgShader(GL_FRAGMENT_SHADER);
	fgShader.load("data/shaders/default.frag");

	GLuint shaderProgram = CreateProgram(vxShader,fgShader,"outColor");
	glUseProgram(shaderProgram);
	
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib,2,GL_FLOAT,GL_FALSE,4*sizeof(float),0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

	glm::mat4 m(1.0f);
	glActiveTexture(GL_TEXTURE0);
	TextureAtlas atlas;
	atlas.loadFromFile("data/atlas.bin");
	Texture t = atlas.findSubTexture("floor1");
	glBindTexture(GL_TEXTURE_2D,*(t.m_texture));
	vertices[2] = t.m_rect.left;
	vertices[3] = t.m_rect.top;
	vertices[6] = t.m_rect.left + t.m_rect.width;
	vertices[7] = t.m_rect.top;
	vertices[10]= t.m_rect.left + t.m_rect.width;
	vertices[11]= t.m_rect.top + t.m_rect.height;
	vertices[14]= t.m_rect.left;
	vertices[15]= t.m_rect.top + t.m_rect.height;
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	glUniform1i(glGetUniformLocation(shaderProgram,"tex"),0);
	GLuint MatrixID = glGetUniformLocation(shaderProgram,"VP");
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
		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&camera.getVP()[0][0]);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1,&ebo.handle);
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
