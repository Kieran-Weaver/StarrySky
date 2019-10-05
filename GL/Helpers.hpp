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
template<typename T>
void genIndexBuffer(T max_sprites, GLBuffer<T>& buffer);
extern template void genIndexBuffer<uint16_t>(uint16_t max_sprites, GLBuffer<uint16_t>& buffer);
struct Vertex{
	float posX;
	float posY;
	uint16_t texX;
	uint16_t texY;
};
#endif
