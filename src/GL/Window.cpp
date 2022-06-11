#define GLFW_INCLUDE_NONE
#include <GL/Window.hpp>
#include <GL/Camera.hpp>
#include <glbinding/gl/gl.h>
#include <GLFW/glfw3.h>
#include <GL/SpriteBatch.hpp>
#include <glbinding/glbinding.h>
#ifndef NO_IMGUI
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_glfw.h"
#endif

using namespace gl;

void GLFWwindowDeleter::operator()(GLFWwindow* ptr){
	glfwDestroyWindow(ptr);
}

void MouseEnterCB(GLFWwindow* window, int entered){
	auto* ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (entered){
		ws->mouseOn = true;
	} else {
		ws->mouseOn = false;
	}
}

void MousePosCB(GLFWwindow* window, double xpos, double ypos){
	auto* ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (ws->mouseOn && ws->cursorCB){
		ws->cursorCB(xpos, ypos);
	}
}

void MouseButtonCB(GLFWwindow* window, int button, int action, int mods){
	auto* ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (ws->mouseOn && ws->mouseCB){
		ws->mouseCB(button, action, mods);
	}
}
	
void MouseScrollCB(GLFWwindow* window, double xoff, double yoff){
	auto* ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (ws->mouseOn && ws->scrollCB){
		ws->scrollCB(xoff, yoff);
	}
}

void KeyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GLFW_TRUE);
	}
	auto * ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS){
		ws->keyboardState[key] = true;
	}else if (action == GLFW_RELEASE){
		ws->keyboardState[key] = false;
	}
#ifndef NO_IMGUI
	ImGui_ImplGlfw_KeyCallback(window,key,scancode,action,mods);
#endif
}
Window::Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname, bool offscreen){
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
	if (offscreen){
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	}
	windowImpl.reset(glfwCreateWindow(w, h, windowname.c_str(), nullptr, nullptr));
	if (!windowImpl){
		glfwTerminate();
		std::exit(0);
	}
	this->makeCurrent();
	glbinding::initialize(glfwGetProcAddress);
	glfwSwapInterval(1);
#ifndef NO_IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(windowImpl.get(),true);
	io.Fonts->AddFontFromFileTTF(fontfile.c_str(),20.f);
#endif
	glfwSetWindowUserPointer(windowImpl.get(), &internal_state);

	glfwSetKeyCallback(windowImpl.get(), KeyboardCB);	
	glfwSetCursorEnterCallback(windowImpl.get(), MouseEnterCB);
	glfwSetCursorPosCallback(windowImpl.get(), MousePosCB);	
	glfwSetMouseButtonCallback(windowImpl.get(), MouseButtonCB);
	glfwSetScrollCallback(windowImpl.get(), MouseScrollCB);
}
Window::~Window(){
	if (this->isOpen()){
		this->close();
	}
#ifndef NO_IMGUI
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif
	glfwTerminate();
}
void Window::startFrame() const{
	this->makeCurrent();
	glfwPollEvents();
#ifndef NO_IMGUI
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
#endif
	gl::glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Window::makeCurrent() const{
	glfwMakeContextCurrent(windowImpl.get());
}
void Window::endFrame() const{
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
void Window::Draw(const DrawList& dlist) const{
	for (const auto& command : dlist){
		if (auto pval = std::get_if<ConfCommand>(&command)){
			this->Configure(*pval);
		} else if (auto pval = std::get_if<DrawCommand>(&command)){
			this->Draw(*pval);
		}
	}
}
void Window::Configure(const ConfCommand& cmd) const{
	for (const auto& fpair : cmd.enable_flags){
		if (fpair.second){
			glEnable(fpair.first);
		} else {
			glDisable(fpair.first);
		}
	}
	if (cmd.blend_func) gl::glBlendFunc(cmd.blend_func.value()[0], cmd.blend_func.value()[1]);
	if (cmd.blend_equation) gl::glBlendEquation(cmd.blend_equation.value());
	if (cmd.cull_face) gl::glCullFace(cmd.cull_face.value());
	if (cmd.depth_func) gl::glDepthFunc(cmd.depth_func.value());
	if (cmd.depth_range) gl::glDepthRange(cmd.depth_range.value()[0], cmd.depth_range.value()[1]);
	if (cmd.logic_op) gl::glLogicOp(cmd.logic_op.value());
	if (cmd.line_width) gl::glLineWidth(cmd.line_width.value());
	if (cmd.sample_mask) gl::glSampleMaski(cmd.sample_mask.value()[0], cmd.sample_mask.value()[1]);
	if (cmd.stencil_func)
		gl::glStencilFunc(cmd.stencil_func.value().func, cmd.stencil_func.value().ref, cmd.stencil_func.value().mask);
	if (cmd.stencil_op) gl::glStencilOp(cmd.stencil_op.value()[0], cmd.stencil_op.value()[1], cmd.stencil_op.value()[2]);
	if (cmd.polygon_offset) gl::glPolygonOffset(cmd.polygon_offset.value()[0], cmd.polygon_offset.value()[1]);
	if (cmd.primitive_restart) gl::glPrimitiveRestartIndex(cmd.primitive_restart.value());
	if (cmd.sample_coverage) gl::glSampleCoverage(cmd.sample_coverage.value().first, cmd.sample_coverage.value().second);
}
void Window::Draw(const DrawCommand& drawcomm) const{
	drawcomm.program->bind();
	drawcomm.VAO->bind();
	for (auto& buff : drawcomm.bound_buffers){
		buff.buff.get().bind();
		if (buff.size){
			buff.buff.get().update(buff.data, buff.size, buff.position);
		}
	}
	if (drawcomm.camera_override){
		this->setCamera(&(drawcomm.camera_override.value()[0][0]), drawcomm.program->getCameraIdx());
	} else {
		this->setCamera(&(this->getWindowState().camera->getVP()[0][0]), drawcomm.program->getCameraIdx());
	}
	std::unordered_map<Draw::Primitive, gl::GLenum> modes = { 
		{Draw::Points, GL_POINTS},
		{Draw::Lines, GL_LINES},
		{Draw::LineStrip, GL_LINE_STRIP},
		{Draw::LineLoop, GL_LINE_LOOP},
		{Draw::Triangles, GL_TRIANGLES},
		{Draw::TriangleStrip, GL_TRIANGLE_STRIP},
		{Draw::TriangleFan, GL_TRIANGLE_FAN}
	};
	std::unordered_map<Draw::IdxType, gl::GLenum> idxtypes = {
		{Draw::Byte, GL_UNSIGNED_BYTE},
		{Draw::Short, GL_UNSIGNED_SHORT},
		{Draw::Int, GL_UNSIGNED_INT}
	};
	for (auto& dcall : drawcomm.calls){
		if (dcall.callback){
			dcall.callback(dcall.callback_opts[0], dcall.callback_opts[1]);
		} else {
			bool instanced = (dcall.instances >= 0);
			bool indexed = (dcall.idxType != Draw::None);
			gl::GLenum dtype = static_cast<gl::GLenum>(modes[dcall.type]);
			gl::GLenum itype = static_cast<gl::GLenum>(idxtypes[dcall.idxType]);
			if (dcall.clip_rect){
				const Rect<int>& cliprect = dcall.clip_rect.value();
				gl::glScissor(cliprect.left, cliprect.top, cliprect.right - cliprect.left, cliprect.bottom - cliprect.top);
			}
			for (size_t i = 0; i < dcall.textures.size(); i++){
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(dcall.textures[i].type, dcall.textures[i].m_texture);
			}
			if (indexed && instanced) {
				if (dcall.baseVertex > 0){
					glDrawElementsInstancedBaseVertex(dtype, dcall.vtxCount, itype, reinterpret_cast<const void*>(dcall.idxOffset), dcall.instances, dcall.baseVertex);
				} else {
					glDrawElementsInstanced(dtype, dcall.vtxCount, itype, reinterpret_cast<const void*>(dcall.idxOffset), dcall.instances);
				}
			} else if (instanced) {
				glDrawArraysInstanced(dtype, dcall.vtxOffset, dcall.vtxCount, dcall.instances);
			} else if (indexed){
				if (dcall.baseVertex > 0){
					glDrawElementsBaseVertex(dtype, dcall.vtxCount, itype, reinterpret_cast<const void*>(dcall.idxOffset), dcall.baseVertex);
				} else {
					glDrawElements(dtype, dcall.vtxCount, itype, reinterpret_cast<const void*>(dcall.idxOffset));
				}
			} else {
				glDrawArrays(dtype, dcall.vtxOffset, dcall.vtxCount);
			}
		}
	}
}

void Window::setCamera(const float* data, int32_t position) const{
	if (position < 0){
		glBufferSubData(static_cast<gl::GLenum>(GL_UNIFORM_BUFFER), -1 - position, sizeof(float) * 16, data);  
	} else {
		glUniformMatrix4fv(position, 1, false, data);
	}
}
