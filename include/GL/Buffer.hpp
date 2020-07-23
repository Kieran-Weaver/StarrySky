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
	void bind(uint32_t texType); // For texture buffers
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
	void setType(uint32_t t){
		this->type = t;
	}
	operator bool() const{
		return this->handle && this->size && this->type;
	}
private:
	uint32_t handle = 0;
	uint32_t size = 0;
	uint32_t type = 0;
	int32_t texType = -1;
};
#endif
