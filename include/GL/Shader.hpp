#ifndef STARRYSKY_SHADER_HPP
#define STARRYSKY_SHADER_HPP
#include <string>
struct Shader{
	Shader(uint32_t type, const std::string& filename);
	Shader() : m_handle(0), m_type(0), loaded(false){}
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&& other);
	Shader& operator=(Shader&& other);
	~Shader();
	uint32_t m_handle;
	uint32_t m_type;
	bool loaded;
};
#endif
