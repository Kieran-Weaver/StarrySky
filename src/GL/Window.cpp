#include "Window.hpp"
#include "gl.h"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#include "imgui/examples/imgui_impl_opengl3.h"
void GLFWwindowDeleter::operator()(GLFWwindow* ptr){
	glfwDestroyWindow(ptr);
}
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
Window::Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname){
	if (glfwInit() != GLFW_TRUE){
		std::exit(0);
	}
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,GLMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,GLMinor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	windowImpl.reset(glfwCreateWindow(w, h, windowname.c_str(), nullptr, nullptr));
	if (!windowImpl){
		glfwTerminate();
		std::exit(0);
	}
	this->makeCurrent();
	glfwSwapInterval(1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(windowImpl.get(),true);
	ImGui_ImplOpenGL3_Init("#version 150");
	io.Fonts->AddFontFromFileTTF(fontfile.c_str(),20.f);
	glfwSetKeyCallback(windowImpl.get(),key_callback);
	glfwSetWindowUserPointer(windowImpl.get(), &internal_state);
}
Window::~Window(){
	if (this->isOpen()){
		this->close();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
void Window::startFrame() const{
	this->makeCurrent();
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Window::makeCurrent() const{
	glfwMakeContextCurrent(windowImpl.get());
}
void Window::endFrame() const{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	this->makeCurrent();
	glfwSwapBuffers(windowImpl.get());
}
void Window::close() const{
	glfwSetWindowShouldClose(windowImpl.get(),GLFW_TRUE);
}
bool Window::isOpen() const{
	return (glfwWindowShouldClose(windowImpl.get()) == GLFW_FALSE);
}
WindowState& Window::getWindowState() {
	return internal_state;
}
const WindowState& Window::getWindowState() const{
	return internal_state;
}
void Window::getWindowSize(int& width, int& height) const{
	glfwGetFramebufferSize(windowImpl.get(), &width, &height);
}
