#include <GL/Window.hpp>
#include <gl.h>
#include <GLFW/glfw3.h>
struct SpriteBatch;
void GLFWwindowDeleter::operator()(GLFWwindow* ptr){
	glfwDestroyWindow(ptr);
}
Window::Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname){
	(void)fontfile;
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
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	windowImpl.reset(glfwCreateWindow(w, h, windowname.c_str(), nullptr, nullptr));
	if (!windowImpl){
		glfwTerminate();
		std::exit(0);
	}
	this->makeCurrent();
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(windowImpl.get(), &internal_state);
}
Window::~Window(){
	if (this->isOpen()){
		this->close();
	}
	glfwTerminate();
}
void Window::startFrame() const{
	this->makeCurrent();
	glfwPollEvents();
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Window::makeCurrent() const{
	glfwMakeContextCurrent(windowImpl.get());
}
void Window::endFrame(SpriteBatch* batch) const{
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
