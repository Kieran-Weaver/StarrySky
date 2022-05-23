#ifndef STARRYSKY_BUFFER_HPP
#define STARRYSKY_BUFFER_HPP
#include <cstdint>
#include <type_traits>
#include <utility>

namespace gl {
	enum class GLenum : unsigned int;
};

class Buffer{
public:
	Buffer(){}
	Buffer(gl::GLenum type_);
	Buffer(const Buffer& buffer) = delete;
	Buffer(Buffer&& other){
		*this = std::move(other);
	}
	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&&);
	~Buffer();
	void bind() const; // For normal buffers
	void bind(gl::GLenum texType); // For texture buffers
	void bind(uint32_t index, intptr_t offset, intptr_t size) const; // For Uniform, TF, AC, and SS buffers
	template<typename T>
	void update(const T& data, uint64_t position){
		this->update(
			data.data(),
			data.size()*sizeof(typename std::decay<typename std::remove_pointer<decltype(data.data())>::type>::type),
			position
		);
	}
	void update(const void* data, uint64_t size, uint64_t position);
	void setType(gl::GLenum t){
		this->type = t;
	}
	operator bool() const{
		return this->handle && this->size && static_cast<uint32_t>(this->type);
	}
private:
	uint32_t handle = 0;
	uint32_t size = 0;
	gl::GLenum type;
	gl::GLenum texType;
};
#endif
