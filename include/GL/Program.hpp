#ifndef STARRYSKY_PROGRAM_HPP
#define STARRYSKY_PROGRAM_HPP
#include <cstdint>
#include <string>
#include <utility>
struct JSONParser;
class Program{
public:
	Program(const JSONParser& data){
		this->load(data);
	}
	Program(){}
	Program(const Program& other) = delete;
	Program& operator=(const Program& other) = delete;
	Program(Program&& other){
		*this = std::move(other);
	}
	Program& operator=(Program&& other);
	~Program();
	void load(const JSONParser& data);
	void bind();
	uint32_t getUniform(const std::string& name);
	void setUniform(const std::string& name, uint32_t value);
	void bindUBO(const std::string& name, int binding);
	void setAttrib(const std::string& name, uint32_t binding);
	operator bool() const{
		return this->loaded;
	}
	int32_t getCameraIdx(){
		return m_camera;
	}
private:
	uint32_t m_handle = 0;
	int32_t m_camera = -1;
	bool loaded = false;
};
#endif
