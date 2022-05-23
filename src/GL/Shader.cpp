#include <glbinding/gl/gl.h>
#include <GL/Shader.hpp>
#include <file/PlainText.hpp>
#include <string>
#include <vector>
#ifndef NDEBUG
#include <iostream>
#endif

using namespace gl;

Shader::Shader(ShaderType type, const std::string& filename) : m_type(type) {
	switch (type) {
	case ShaderType::VERT:
		m_handle = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ShaderType::FRAG:
		m_handle = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	default:
		return;
		break;
	}
	
	std::string contents = readWholeFile(filename);
	const char* shader_contents = contents.c_str();
	glShaderSource(m_handle,1,&shader_contents,nullptr);
	glCompileShader(m_handle);
#ifndef NDEBUG
	GLboolean success = 0;
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE){
		std::cerr << "Shader compilation failed: " << filename << std::endl;
		GLint maxLength = 0;
		glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_handle, maxLength, &maxLength, &errorLog[0]);
		std::string errorMessage(errorLog.data(), errorLog.size());
		std::cerr << "Message: " << errorMessage << std::endl;
		glDeleteShader(m_handle);
		this->loaded = false;
	}else{
		this->loaded = true;
	}
#else
	this->loaded = true;
#endif
}
Shader::Shader(Shader&& other) : m_handle(other.m_handle), m_type(other.m_type), loaded(other.loaded){
	other.m_handle = 0;
	other.loaded = false;
}
Shader& Shader::operator=(Shader&& other){
	if (this != &other){
		glDeleteShader(m_handle);
		this->m_handle = other.m_handle;
		this->m_type = other.m_type;
		this->loaded = other.loaded;
		other.m_handle = 0;
	}
	return *this;
}
Shader::~Shader(){
	glDeleteShader(m_handle);
	loaded = false;
}
