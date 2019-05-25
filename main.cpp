#include "gl31.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <zlib.h>
#include <cstring>
#include "GL/Shader.hpp"
#include "GL/Camera.hpp"
struct PODvec2{
	float x;
	float y;
};
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
int loadDDSgz(const std::string& path,GLuint& m_texture){
	glBindTexture(GL_TEXTURE_2D,m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gzFile gzhandle = gzopen(path.c_str(),"rb");
	char * header = new char[129];
	gzread(gzhandle, header, 128);
	header[128] = '\0';
	if (strncmp(header,"DDS ",4)!=0){
		delete[] header;
		gzclose(gzhandle);
		return -1;
	}else{
		uint32_t height = *reinterpret_cast<uint32_t*>(header+12);
		uint32_t width = *reinterpret_cast<uint32_t*>(header+16);
		uint32_t mipmapcount = *reinterpret_cast<uint32_t*>(header+28);
		uint32_t fourCC = *reinterpret_cast<uint32_t*>(header+84);
		uint32_t blockSize = 16;
		uint32_t size = ((width + 3)/4)*((height + 3)/4)*blockSize;
		uint32_t format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		delete[] header;
		switch(fourCC){
			case 0x31545844: // DXT1
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSize = 8;
				break;
			case 0x33545844: // DXT3
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case 0x35545844: // DXT5
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			default:
				std::cout << "Not DXT" << std::endl;
				return -1;
				break;
		}
		uint8_t *data = new uint8_t[size];
		for (uint32_t level=0;level<mipmapcount;level++){
			std::cout << size << std::endl;
			delete[] data;
			data = new uint8_t[size];
			gzread(gzhandle,data,size);
			glCompressedTexImage2D(GL_TEXTURE_2D,level,format,width,height,0,size,data);
			width = std::max(width/2, 1U);
			height = std::max(height/2,1U);
			size = ((width + 3)/4)*((height + 3)/4)*blockSize;
		}
		gzclose(gzhandle);
		delete[] data;
	}
	return 0;
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
	window = glfwCreateWindow(1920,1080,"Starry Sky", monitor, NULL);
	if (!window){
		glfwTerminate();
		return 2;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window,&camera);
	float vertices[] = {
		 0.f,0.f,0.0f,0.0f,
		 2000.f,0.f,1.0f,0.0f,
		 2000.f,1000.f,1.0f,1.0f,
		 0.f,1000.f,0.0f,1.0f
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
	glm::mat4 m(1.0f);
	glGenTextures(1,&tex);
	glActiveTexture(GL_TEXTURE0);
	loadDDSgz("atlas0.dds.gz",tex);
	glUniform1i(glGetUniformLocation(shaderProgram,"tex"),0);
	GLuint MatrixID = glGetUniformLocation(shaderProgram,"VP");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glfwSetKeyCallback(window,key_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window)){
	/* Render here */
		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&camera.getVP()[0][0]);
//		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&m[0][0]);
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
