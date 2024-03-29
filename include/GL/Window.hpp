#ifndef STARRYSKY_WINDOW_HPP
#define STARRYSKY_WINDOW_HPP
#include <string>
#include <bitset>
#include <memory>
#include <functional>
#include <GL/DrawCommand.hpp>
#include <GL/Program.hpp>
#define MAX_KEYCODE 1023
class GLFWwindow;
class Camera;
class SpriteBatch;
struct GLFWwindowDeleter{
	void operator()(GLFWwindow *p);
};
struct WindowState{
	Camera* camera;
	std::bitset<MAX_KEYCODE + 1> keyboardState = {};
	bool mouseOn = false;	
	std::function<void(double, double)> cursorCB = nullptr;
	std::function<void(int,int,int)> mouseCB = nullptr;
	std::function<void(double, double)> scrollCB = nullptr;
};
class Window{
public:
	explicit Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname, bool offscreen);
	~Window();
	void startFrame() const;
	void makeCurrent() const;
	void getWindowSize(int& width, int& height) const;
	WindowState& getWindowState();
	const WindowState& getWindowState() const;
	void Draw(const DrawList& dlist) const;
	void endFrame() const;
	void close() const;
	bool isOpen() const;
private:
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> windowImpl;
	void Draw(const DrawCommand& drawcomm) const;
	void Configure(const ConfCommand& confcomm) const;
	void setCamera(const float* data, int32_t position) const;
};
#endif
