#ifndef STARRYSKY_SHADER_HPP
#define STARRYSKY_SHADER_HPP
#include <gl.h>
#include "Helpers.hpp"
class Shader
{
public:
	Shader(GLenum type);
	Shader() : m_handle(0), m_type(0){}
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other);
	Shader& operator=(Shader&& other);
	~Shader();
	void load(const std::string& filename);
	GLuint m_handle;
private:
	GLenum m_type;
	bool loaded = false;
};
GLuint CreateProgram(const std::string& VertexShader, const std::string& FragShader, const std::string& OutputLocation);
GLuint CreateProgram(const Shader& VertexShader, const Shader& FragShader, const std::string& OutputLocation);
#endif
