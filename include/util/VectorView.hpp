#ifndef STARRYSKY_VV_HPP
#define STARRYSKY_VV_HPP
#include <vector>
#include <functional>
template<typename T>
class vector_view{
	using value_type = T;
	vector_view(){}
	vector_view(std::vector<T>& ptr) : ptr_(std::ref(ptr)), end_(ptr.size()){}
	vector_view(std::vector<T>& ptr, std::size_t len) : ptr_(std::ref(ptr)), end_(len){}
	vector_view(std::vector<T>& ptr, std::size_t beg, std::size_t end) : ptr_(std::ref(ptr)), begin_(beg), end_(end){}
	T& operator[](std::size_t i){
		return ptr_[begin_ + i];
	}
	const T& operator[](std::size_t i) const{
		return ptr_[begin_ + i];
	}
	const T* data() noexcept{
		return ptr_.data() + begin_;
	}
	auto size() const noexcept{
		return end_ - begin_;
	}
	auto begin() noexcept{
		return ptr_.begin() + begin_;
	}
	auto end() noexcept{
		return ptr_.begin() + end_;
	}
	std::reference_wrapper<std::vector<T>> ptr_;
	std::size_t begin_ = 0;
	std::size_t end_ = 0;
};
#endif
