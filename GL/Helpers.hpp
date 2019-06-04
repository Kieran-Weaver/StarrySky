#ifndef STARRYSKY_HELPER_HPP
#define STARRYSKY_HELPER_HPP
#include <fstream>
#include <limits>
#include <gl.h>
#include <iostream>
std::string readWholeFile(const std::string& filename);
template<typename T>
struct GLBuffer{
	~GLBuffer(){delete[] data;}
	GLuint handle;
	size_t size;
	T * data;
};
template<typename T>
GLBuffer<T> genIndexBuffer(T max_sprites){
	static_assert(std::is_integral<T>::value, "Integral type required");
	max_sprites -= (max_sprites & 0x03);
	GLBuffer<T> buffer;
	glGenBuffers(1,&buffer.handle);
	buffer.data = new T[(max_sprites*3 + 1)/2];
	buffer.size = (max_sprites*3 + 1)/2;
	size_t index = 0;
	for (T sprite = 0; sprite < max_sprites; sprite+=4){
		buffer.data[index++] = sprite;
		buffer.data[index++] = sprite+1;
		buffer.data[index++] = sprite+2;
		buffer.data[index++] = sprite+2;
		buffer.data[index++] = sprite+3;
		buffer.data[index++] = sprite;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.size*sizeof(T), buffer.data, GL_STATIC_DRAW);
	return buffer;
}
#endif
