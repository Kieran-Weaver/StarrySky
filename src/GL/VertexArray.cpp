#include <GL/VertexArray.hpp>
#include <gl.h>
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
	if (attr.type == GL_FLOAT || attr.normalized){
		glVertexAttribPointer(attr.location,attr.components,attr.type,attr.normalized,attr.stride,reinterpret_cast<void*>(attr.start));
	}else{
		glVertexAttribIPointer(attr.location,attr.components,attr.type,attr.stride,reinterpret_cast<void*>(attr.start));
	}
}
