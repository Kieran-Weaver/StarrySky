#ifndef STARRYSKY_VAO_HPP
#define STARRYSKY_VAO_HPP
#include <cstdint>
#include <utility>

namespace gl {
	enum class GLenum : unsigned int;
};

struct Attrib{
	int32_t location;
	uint32_t components;
	gl::GLenum type;
	bool normalized;
	uint32_t start;
	uint32_t stride;
};
class VertexArray{
public:
	VertexArray();
	VertexArray(const VertexArray& VertexArray) = delete;
	VertexArray(VertexArray&& other){
		*this = std::move(other);
	}
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray& operator=(VertexArray&&);
	~VertexArray();
	void bind();
	void setAttrib(const Attrib& attr);
private:
	uint32_t handle = 0;
};
#endif
