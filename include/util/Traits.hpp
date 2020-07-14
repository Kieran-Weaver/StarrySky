#ifndef STARRYSKY_TRAITS_HPP
#define STARRYSKY_TRAITS_HPP
#include <array>
template<class T>
struct is_std_array:std::is_array<T>{};
template<class T, std::size_t N>
struct is_std_array<std::array<T, N>>:std::true_type{};

template<typename T>
constexpr bool is_json_literal(){
	return std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T, bool>::value || std::is_same<T, std::string>::value;
}
#endif
