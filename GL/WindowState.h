#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H
#include <gl.h>
#include "Camera.hpp"
struct WindowState{
	Camera* camera;
	GLuint MatrixID;
};
#endif
