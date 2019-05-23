#include "gl31.h"
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
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
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(mode->width,mode->height,"Starry Sky", monitor, NULL);
	if (!window){
		glfwTerminate();
		return 2;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window,&camera);
	float vertices[] = {
		 0.f,0.f,0.0f,0.0f,
		 900.f,0.f,2.0f,0.0f,
		 900.f,400.f,2.0f,2.0f,
		 0.f,400.f,0.0f,2.0f
	};
	GLuint vao;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	GLBuffer<uint8_t> ebo = genIndexBuffer<uint8_t>(20);

	Shader vxShader(GL_VERTEX_SHADER);
	vxShader.load("shaders/default.vert");

	Shader fgShader(GL_FRAGMENT_SHADER);
	fgShader.load("shaders/default.frag");

	GLuint shaderProgram = CreateProgram(vxShader,fgShader,"outColor");
	glUseProgram(shaderProgram);
	
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib,2,GL_FLOAT,GL_FALSE,4*sizeof(float),0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

	GLuint tex;
	glGenTextures(1,&tex);
	int width, height;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,tex);
	unsigned char* image = stbi_load("img_test.png",&width,&height,0,STBI_rgb_alpha);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glUniform1i(glGetUniformLocation(shaderProgram,"tex"),0);
	GLuint MatrixID = glGetUniformLocation(shaderProgram,"VP");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glfwSetKeyCallback(window,key_callback);
	while (!glfwWindowShouldClose(window)){
	/* Render here */
		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&camera.getVP()[0][0]);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0);
	/* Swap front and back buffers */
		glfwSwapBuffers(window);
	/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteTextures(1,&tex);
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1,&ebo.handle);
	glDeleteBuffers(1,&vbo);
	glDeleteVertexArrays(1,&vao);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
