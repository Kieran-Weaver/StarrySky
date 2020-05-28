#ifndef STARRYSKY_SHADER_HPP
#define STARRYSKY_SHADER_HPP
#include <gl.h>
#include <string>
class Shader
{
public:
	Shader(GLenum type, const std::string& filename);
	Shader() : m_handle(0), m_type(0), loaded(false){}
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other);
	Shader& operator=(Shader&& other);
	~Shader();
	GLuint m_handle;
	GLenum m_type;
	bool loaded;
};
GLuint CreateProgram(const Shader& VertexShader, const Shader& GeomShader, const Shader& FragShader, const std::string& OutputLocation);
#endif
