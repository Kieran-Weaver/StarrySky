#ifndef STARRYSKY_SHADER_HPP
#define STARRYSKY_SHADER_HPP
#include <fstream>
#include <gl.h>
#include "Helpers.hpp"
class Shader
{
public:
	Shader(GLenum type);
	~Shader();
	void load(const std::string& filename);
	GLenum m_type;
	GLuint m_handle;
};
GLuint CreateProgram(const std::string& VertexShader, const std::string& FragShader, const std::string& OutputLocation);
GLuint CreateProgram(const Shader& VertexShader, const Shader& FragShader, const std::string& OutputLocation);
#endif
