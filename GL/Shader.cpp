#include "Shader.hpp"
#include <fstream>
#include <array>
#include <iostream>
Shader::Shader(GLenum type) : m_handle(0), m_type(type) {}
Shader::Shader(Shader&& other) : m_handle(other.m_handle), m_type(other.m_type){
	other.m_handle = 0;
}
Shader& Shader::operator=(Shader&& other){
	if (this != &other){
		glDeleteShader(m_handle);
		this->m_handle = other.m_handle;
		this->m_type = other.m_type;
		other.m_handle = 0;
	}
	return *this;
}
Shader::~Shader(){
	glDeleteShader(m_handle);
}
void Shader::load(const std::string& filename){
	m_handle = glCreateShader(this->m_type);
	std::string contents = readWholeFile(filename);
	const char* shader_contents = contents.c_str();
	glShaderSource(m_handle,1,&shader_contents,nullptr);
	glCompileShader(m_handle);
	GLint success = 0;
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
		loaded = false;
	}else{
		loaded = true;
	}
}
GLuint CreateProgram(const std::string& VertexShader, const std::string& FragShader, const std::string& OutputLocation){
	std::array<Shader,2> shaders = {GL_VERTEX_SHADER,GL_FRAGMENT_SHADER};
	shaders[0].load(VertexShader);
	shaders[1].load(FragShader);
	return CreateProgram(shaders[0],shaders[1],OutputLocation);
}
GLuint CreateProgram(const Shader& VertexShader, const Shader& FragShader, const std::string& OutputLocation){
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, VertexShader.m_handle);
	glAttachShader(shaderProgram, FragShader.m_handle);
	glBindFragDataLocation(shaderProgram,0,OutputLocation.c_str());
	glLinkProgram(shaderProgram);
	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE){
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(shaderProgram);
		std::string errorMessage(infoLog.data(), infoLog.size());
		std::cerr << "Message: " << errorMessage << std::endl;
		return 0;
	}else{
		return shaderProgram;
	}
}
GLuint CreateProgram(const Shader& VertexShader, const Shader& GeomShader, const Shader& FragShader, const std::string& OutputLocation){
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, VertexShader.m_handle);
	glAttachShader(shaderProgram, GeomShader.m_handle);
	glAttachShader(shaderProgram, FragShader.m_handle);
	glBindFragDataLocation(shaderProgram,0,OutputLocation.c_str());
	glLinkProgram(shaderProgram);
	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE){
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(shaderProgram);
		std::string errorMessage(infoLog.data(), infoLog.size());
		std::cerr << "Message: " << errorMessage << std::endl;
		return 0;
	}else{
		return shaderProgram;
	}
}
