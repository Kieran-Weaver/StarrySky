#include <GL/Buffer.hpp>
#include <gl.h>
uint32_t nextPow2(uint32_t v){
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}
Buffer::Buffer(uint32_t type_) : type(type_){
	glGenBuffers(1, &handle);
}
Buffer& Buffer::operator=(Buffer&& other){
	this->handle = std::move(other.handle);
	this->size = std::move(other.size);
	this->type = std::move(other.type);
	other.type = 0;
	other.size = 0;
	other.handle = 0;
	return *this;
}
Buffer::~Buffer(){
	glDeleteBuffers(1, &handle);
}
void Buffer::bind() const{
	glBindBuffer(this->type, this->handle);
}
void Buffer::bind(uint32_t texType){
	this->texType = texType;
	this->bind();
	glTexBuffer(this->type, texType, this->handle);
}
void Buffer::bind(uint32_t index, intptr_t offset, intptr_t size) const {
	glBindBufferRange(this->type, index, this->handle, offset, size);
}
void Buffer::update(const void* data, uint64_t size, uint64_t position){
	if (size + position > this->size){
		uint32_t new_handle;
		uint32_t new_size = nextPow2(size + position + 1);
		glGenBuffers(1, &new_handle);
		glBindBuffer(GL_COPY_READ_BUFFER, this->handle);
		glBindBuffer(GL_COPY_WRITE_BUFFER, new_handle);

		glBufferData(GL_COPY_WRITE_BUFFER, this->size, nullptr, GL_DYNAMIC_COPY);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->size);

		glBufferData(GL_COPY_READ_BUFFER, new_size, nullptr, GL_DYNAMIC_COPY);
		glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, this->size);

		glDeleteBuffers(1, &new_handle);
		this->size = new_size;
	}
	this->bind();
	glBufferSubData(this->type, position, size, data);
}
