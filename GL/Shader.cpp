#include "Shader.hpp"
Shader::Shader(GLenum type) : m_type(type){}
Shader::~Shader(){
	glDeleteShader(m_handle);
}
void Shader::load(const std::string& filename){
	m_handle = glCreateShader(this->m_type);
	std::string contents = readWholeFile(filename);
	const char* shader_contents = contents.c_str();
	glShaderSource(m_handle,1,&shader_contents,0);
	glCompileShader(m_handle);
}
GLuint CreateProgram(const std::string& VertexShader, const std::string& FragShader, const std::string& OutputLocation){
	Shader shaders[2] = {Shader(GL_VERTEX_SHADER), Shader(GL_FRAGMENT_SHADER)};
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
