#ifndef STARRYSKY_JSONREADER_HPP
#define STARRYSKY_JSONREADER_HPP
#include <rapidjson/fwd.h>
#include <visit_struct/visit_struct.hpp>
#include <util/Traits.hpp>
#include <unordered_map>
#include <vector>
#include <array>
#include <memory>
#include <string>
struct DocDeleter{
	void operator()(rapidjson::Document*);
};
class JSONParser{
public:
	JSONParser(rapidjson::Value& ref) : internal(ref){}
	operator rapidjson::Value&() const {
		return internal;
	}
	JSONParser operator[](const char* key) const;
	JSONParser operator[](std::string_view key) const;
	JSONParser operator[](int key) const;
	bool HasKey(const std::string& key) const;
	bool IsString() const;
	std::vector<std::string_view> getKeys() const;
	int size() const;
	template<typename T, std::enable_if_t< \
	!(visit_struct::traits::is_visitable<T>::value || is_json_literal<T>() || is_std_array<T>::value) \
	, int> = 0>
	operator T() const;
	
	template<typename T,  std::enable_if_t<visit_struct::traits::is_visitable<T>::value, int> = 0>
	operator T() const{
		T data;
		visit_struct::for_each(data, [&](const char* name, auto& value){
			value = static_cast<std::decay_t<decltype(value)>>((*this)[std::string_view(name)]);
		});
		return data;
	}
	
	template<typename T>
	operator std::unordered_map<std::string, T>() const{
		std::unordered_map<std::string, T> data;
		std::vector<std::string_view> keys = this->getKeys();
		for (auto key : keys){
			data[key] = static_cast<T>((*this)[key]);
		}
		return data;
	}
	
	template<typename T>
	operator std::vector<T>() const{
		std::vector<T> data;
		for (int i = 0; i < this->size(); i++){
			data.push_back((*this)[i]);
		}
		return data;
	}
	
	template<typename T, std::size_t N>
	operator std::array<T,N>() const{
		std::array<T,N> data;
		for (size_t i = 0; i < data.size(); i++){
			data[i] = static_cast<T>((*this)[i]);
		}
		return data;
	}
	
	template<typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
	operator T() const{
		return static_cast<int64_t>(*this);
	}
	
	operator bool() const {
		return static_cast<int64_t>(*this);
	}
	
	template<typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
	operator T() const{
		return static_cast<double>(*this);
	}

	operator int64_t() const;
	
	operator double() const;

	operator std::string() const;
private:
	rapidjson::Value& internal;
};

class JSONReader{
public:
	JSONReader() = default;
	JSONReader(const char* text);
	JSONParser operator[](const std::string& key);
private:
	std::unique_ptr<rapidjson::Document, DocDeleter> document;
};
#endif
