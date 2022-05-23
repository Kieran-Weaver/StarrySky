#include <GL/VertexArray.hpp>
#include <glbinding/gl/gl.h>

using namespace gl;

VertexArray::VertexArray(){
	glGenVertexArrays(1, &handle);
}
VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &handle);
}
VertexArray& VertexArray::operator=(VertexArray&& other){
	this->handle = std::move(other.handle);
	other.handle = 0;
	return *this;
}
void VertexArray::bind(){
	glBindVertexArray(this->handle);
}
void VertexArray::setAttrib(const Attrib& attr){
	this->bind();
	glEnableVertexAttribArray(attr.location);
	
	gl::GLenum type = static_cast<GLenum>(attr.type);
	void* start = reinterpret_cast<void*>(attr.start);
	
	if (type == GL_FLOAT || attr.normalized){
		glVertexAttribPointer(attr.location,attr.components,type,attr.normalized,attr.stride,start);
	}else{
		glVertexAttribIPointer(attr.location,attr.components,type,attr.stride,start);
	}
}
