#ifndef STARRYSKY_SHADER_HPP
#define STARRYSKY_SHADER_HPP
#include <string>

enum class ShaderType {
	VERT,
	FRAG,
	NONE
};

struct Shader{
	Shader(ShaderType type, const std::string& filename);
	Shader() : m_handle(0), m_type(ShaderType::NONE), loaded(false){}
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other);
	Shader& operator=(Shader&& other);
	~Shader();
	uint32_t m_handle;
	ShaderType m_type;
	bool loaded;
};
#endif
