#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H
#include <gl.h>
#include <glfw3.h>
#include <bitset>
class Camera;
struct WindowState{
	Camera* camera;
	std::bitset<GLFW_KEY_LAST+1> keyboardState;
	GLuint MatrixID;
};
#endif
