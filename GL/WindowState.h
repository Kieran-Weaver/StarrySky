#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H
#include <gl.h>
#include <bitset>
#include <GLFW/glfw3.h>
#include "Camera.hpp"
struct WindowState{
	Camera* camera;
	std::bitset<GLFW_KEY_LAST+1> keyboardState;
	GLuint MatrixID;
};
#endif
