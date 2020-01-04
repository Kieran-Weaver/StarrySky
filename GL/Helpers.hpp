#ifndef STARRYSKY_HELPER_HPP
#define STARRYSKY_HELPER_HPP
#include <gl.h>
#include <string>
#include <vector>
std::string readWholeFile(const std::string& filename);
template<typename T>
struct GLBuffer{
	GLBuffer(){
		glGenBuffers(1, &this->m_handle);
	}
	~GLBuffer(){
		glDeleteBuffers(1, &this->m_handle);
	}
	GLBuffer(const GLBuffer&) = delete;
	GLBuffer& operator=(const GLBuffer&) = delete;
	GLBuffer(GLBuffer&& other) : m_handle(other.m_handle), data(other.data){
		other.m_handle = 0;
	}
	GLBuffer& operator=(GLBuffer&& other){
		if (this != &other){
			glDeleteBuffers(1,&this->m_handle);
			this->m_handle = other.m_handle;
			this->data = other.data;
			other.m_handle = 0;
		}
		return *this;
	}
	GLuint m_handle;
	std::vector<T> data;
};
struct GLRect2D{
	uint16_t texRect[4];
	float packedMat[4];
	float sprPos[2];
};
#endif
