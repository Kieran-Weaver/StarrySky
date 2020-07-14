#include <GL/Program.hpp>
#include <gl.h>
#ifndef NDEBUG
#include <iostream>
#endif
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
	Shader vtx(GL_VERTEX_SHADER, data["vxFile"]), geom, frag(GL_FRAGMENT_SHADER, data["fgFile"]);
	std::string output = data["output"];
	this->m_handle = glCreateProgram();
	glAttachShader(this->m_handle, vtx.m_handle);
	if (data["usesGS"]){
		geom = Shader(GL_GEOMETRY_SHADER, data["gsFile"]);
		glAttachShader(this->m_handle, geom.m_handle);
	}
	glAttachShader(this->m_handle, frag.m_handle);
	glBindFragDataLocation(this->m_handle,0,output.c_str());
	for (auto& pnode : data["layout"].GetArray()){
		this->setAttrib(pnode["name"].GetString(), pnode["location"].GetInt64());
	}
	glLinkProgram(this->m_handle);
#ifndef NDEBUG
	GLint isLinked = 0;
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
	if (static_cast<rapidjson::Value&>(data["camera"]).IsString()){
		this->m_camera = glGetUniformLocation(this->m_handle, static_cast<std::string>(data["camera"]).c_str());
	} else {
		this->m_camera = (-1 * static_cast<int>(data["camera"])) - 1;
	}
}
uint32_t Program::getUniform(const std::string& name){
	return glGetUniformLocation(this->m_handle, name.c_str());
}
void Program::bindUBO(const std::string& name, int binding){
	int idx = glGetUniformBlockIndex(this->m_handle, name.c_str());
	if (idx != GL_INVALID_INDEX){
		glUniformBlockBinding(this->m_handle, idx, binding);
	}
}
void Program::setAttrib(const std::string& name, uint32_t binding){
	glBindAttribLocation(this->m_handle, binding, name.c_str());
}
