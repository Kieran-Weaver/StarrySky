#ifndef STARRYSKY_BUFFER_HPP
#define STARRYSKY_BUFFER_HPP
#include <util/VectorView.hpp>
#include <cstdint>
class Buffer{
public:
	Buffer() : Buffer(0){}
	Buffer(uint32_t type_);
	Buffer(const Buffer& buffer) = delete;
	Buffer(Buffer&& other){
		*this = std::move(other);
	}
	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&&);
	~Buffer();
	void bind() const; // For normal buffers
	void bind(uint32_t index, intptr_t offset, intptr_t size) const; // For Uniform, TF, AC, and SS buffers
	template<typename T>
	void update(const T& data, uint64_t position){
		this->update(data.data(), data.size(), position);
	}
	void update(const void* data, uint64_t size, uint64_t position);
	void setType(uint32_t t){
		this->type = t;
	}
private:
	uint32_t handle = 0;
	uint32_t size = 0;
	uint32_t type = 0;
};
#endif
