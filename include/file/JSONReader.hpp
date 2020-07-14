#ifndef STARRYSKY_JSONREADER_HPP
#define STARRYSKY_JSONREADER_HPP
#include <rapidjson/document.h>
#include <visit_struct/visit_struct.hpp>
#include <util/Traits.hpp>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>
class JSONParser{
public:
	JSONParser(rapidjson::Value& ref) : internal(ref){}
	operator rapidjson::Value&() const {
		return internal;
	}
	JSONParser operator[](const std::string& key) const{
		return {internal[key.c_str()]};
	}
	JSONParser operator[](int key) const{
		return {internal[key]};
	}
	const auto GetArray() const{
		return internal.GetArray();
	}
	const auto GetObject() const{
		return internal.GetObject();
	}
	template<typename T, std::enable_if_t< \
	!(visit_struct::traits::is_visitable<T>::value || is_json_literal<T>() || is_std_array<T>::value) \
	, int> = 0>
	operator T() const;
	
	template<typename T,  std::enable_if_t<visit_struct::traits::is_visitable<T>::value, int> = 0>
	operator T() const{
		T data;
		visit_struct::for_each(data, [&](const char* name, auto& value){
			value = static_cast<std::decay_t<decltype(value)>>((*this)[name]);
		});
		return data;
	}
	
	template<typename T>
	operator std::unordered_map<std::string, T>() const{
		std::unordered_map<std::string, T> data;
		for (auto& m : internal.GetObject()){
			const std::string& key = m.name.GetString();
			data[key] = static_cast<T>((*this)[key]);
		}
		return data;
	}
	
	template<typename T>
	operator std::vector<T>() const{
		std::vector<T> data;
		for (auto& v : this->GetArray()){
			data.push_back((*this)[data.size()]);
		}
		return data;
	}
	
	template<typename T, std::size_t N>
	operator std::array<T,N>() const{
		std::array<T,N> data;
		for (int i = 0; i < data.size(); i++){
			data[i] = static_cast<T>((*this)[i]);
		}
		return data;
	}

	template<typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
	operator T() const{
		if (internal.IsBool()){
			return internal.GetBool();
		} else {
			return internal.GetInt64();
		}
	}
	
	template<typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
	operator T() const{
		return internal.GetDouble();
	}

	template<typename T, std::enable_if_t<std::is_same<typename std::decay<T>::type, std::string>::value, int> = 0>
	operator T() const{
		return internal.GetString();
	}
private:
	rapidjson::Value& internal;
};

class JSONReader{
public:
	JSONReader() = default;
	JSONReader(const char* text){
		document.Parse(text);
	}
	JSONParser operator[](const std::string& key){
		return {document[key.c_str()]};
	}
private:
	rapidjson::Document document;
};
#endif
