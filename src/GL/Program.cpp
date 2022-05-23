#include <GL/Program.hpp>
#include <GL/Shader.hpp>
#include <file/JSONReader.hpp>
#include <glbinding/gl/gl.h>
#include <vector>
#ifndef NDEBUG
#include <iostream>
#endif

using namespace gl;

Program& Program::operator=(Program&& other){
	this->m_handle = other.m_handle;
	this->m_camera = other.m_camera;
	this->loaded = other.loaded;
	other.m_handle = 0;
	other.loaded = false;
	return *this;
}
Program::~Program(){
	glDeleteProgram(this->m_handle);
}
void Program::bind(){
	glUseProgram(this->m_handle);
}
void Program::load(const JSONParser& data){
	if (loaded){
		glDeleteProgram(this->m_handle);
	}
	Shader vtx(ShaderType::VERT, data["vxFile"]), frag(ShaderType::FRAG, data["fgFile"]);
	std::string output = data["output"];
	this->m_handle = glCreateProgram();
	glAttachShader(this->m_handle, vtx.m_handle);
	glAttachShader(this->m_handle, frag.m_handle);
	glBindFragDataLocation(this->m_handle,0,output.c_str());
	for (int i = 0; i < data["layout"].size(); i++){
		auto pnode = data["layout"][i];
		this->setAttrib(static_cast<std::string>(pnode["name"]), static_cast<int64_t>(pnode["location"]));
	}
	glLinkProgram(this->m_handle);
#ifndef NDEBUG
	GLboolean isLinked = 0;
	glGetProgramiv(this->m_handle, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE){
		GLint maxLength = 0;
		glGetProgramiv(this->m_handle, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(this->m_handle, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(this->m_handle);
		std::string errorMessage(infoLog.data(), infoLog.size());
		std::cerr << "Message: " << errorMessage << std::endl;
		this->loaded = false;
	}else{
		this->loaded = true;
	}
#else
	this->loaded = true;
#endif
	if (data["camera"].IsString()){
		this->m_camera = glGetUniformLocation(this->m_handle, static_cast<std::string>(data["camera"]).c_str());
	} else {
		this->m_camera = (-1 * static_cast<int>(data["camera"])) - 1;
	}
}
uint32_t Program::getUniform(const std::string& name){
	return glGetUniformLocation(this->m_handle, name.c_str());
}
void Program::setUniform(const std::string& name, uint32_t value){
	glUniform1i(this->getUniform(name), value);
}
void Program::bindUBO(const std::string& name, int binding){
	uint32_t idx = glGetUniformBlockIndex(this->m_handle, name.c_str());
	if (idx != GL_INVALID_INDEX){
		glUniformBlockBinding(this->m_handle, idx, binding);
	}
}
void Program::setAttrib(const std::string& name, uint32_t binding){
	glBindAttribLocation(this->m_handle, binding, name.c_str());
}
