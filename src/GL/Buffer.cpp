#include <GL/Buffer.hpp>
#include <util/Clib.hpp>
#include <glbinding/gl/gl.h>

using namespace gl;

Buffer::Buffer(gl::GLenum type_) : type(type_){
	glGenBuffers(1, &handle);
}
Buffer& Buffer::operator=(Buffer&& other){
	this->handle = std::move(other.handle);
	this->size = std::move(other.size);
	this->type = std::move(other.type);
	other.type = static_cast<gl::GLenum>(0);
	other.size = 0;
	other.handle = 0;
	return *this;
}
Buffer::~Buffer(){
	if (static_cast<bool>(this->type))
		glDeleteBuffers(1, &handle);
}
void Buffer::bind() const{
	glBindBuffer(this->type, this->handle);
}
void Buffer::bind(gl::GLenum texType){
	this->texType = texType;
	this->bind();
	glTexBuffer(this->type, texType, this->handle);
}
void Buffer::bind(uint32_t index, intptr_t offset, intptr_t size) const {
	glBindBufferRange(this->type, index, this->handle, offset, size);
}
void Buffer::update(const void* data, uint64_t size, uint64_t position){
	if (this->size){
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
	} else {
		glBindBuffer(GL_COPY_WRITE_BUFFER, this->handle);
		glBufferData(GL_COPY_WRITE_BUFFER, size + position, nullptr, GL_DYNAMIC_DRAW);
	}
	this->bind();
	glBufferSubData(static_cast<GLenum>(this->type), position, size, data);
}
