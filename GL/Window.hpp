#ifndef STARRYSKY_WINDOW
#define STARRYSKY_WINDOW
#include <string>
#include <bitset>
#include <memory>
#define MAX_KEYCODE 1023
class GLFWwindow;
struct GLFWwindowDeleter{
	void operator()(GLFWwindow *p);
};
class Camera;
struct WindowState{
	Camera* camera;
	std::bitset<MAX_KEYCODE + 1> keyboardState;
};
class Window{
public:
	explicit Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname);
	~Window();
	void startFrame() const;
	void makeCurrent() const;
	void setWindowState(const WindowState& ws);
	void getWindowSize(int& width, int& height) const;
	WindowState getWindowState() const;
	void endFrame() const;
	void close() const;
	bool isOpen() const;
private:
	WindowState internal_state;
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> windowImpl;
};
#endif
