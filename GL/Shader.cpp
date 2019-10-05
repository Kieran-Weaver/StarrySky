#include "Shader.hpp"
#include <fstream>
#include <array>
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
	loaded = true;
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
	return shaderProgram;
}
