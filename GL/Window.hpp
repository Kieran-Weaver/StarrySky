#ifndef STARRYSKY_WINDOW
#define STARRYSKY_WINDOW
#include <string>
#include <memory>
class GLFWwindow;
struct GLFWwindowDeleter{
	void operator()(GLFWwindow *p);
};
class WindowState;
class Window{
public:
	explicit Window(int w, int h, int GLMajor, int GLMinor, const std::string& fontfile, const std::string& windowname);
	~Window();
	void startFrame() const;
	void makeCurrent() const;
	void setWindowState(WindowState* ws);
	WindowState* getWindowState() const;
	void endFrame() const;
	void close() const;
	bool isOpen() const;
private:
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> windowImpl;
};
#endif