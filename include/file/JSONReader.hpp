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
	JSONParser operator[](const std::string& key){
		return {internal[key.c_str()]};
	}
	JSONParser operator[](int key){
		return {internal[key]};
	}
	const auto GetArray(){
		return internal.GetArray();
	}
	const auto GetObject(){
		return internal.GetObject();
	}
	template<typename T, std::enable_if_t< \
	!(visit_struct::traits::is_visitable<T>::value || is_json_literal<T>() || is_std_array<T>::value) \
	, int> = 0>
	void load(T& data);
	
	template<typename T,  std::enable_if_t<visit_struct::traits::is_visitable<T>::value, int> = 0>
	void load(T& data){
		visit_struct::for_each(data, [&](const char* name, auto& value){
			JSONParser j(internal[name]);
			j.load(value);
		});
	}
	
	template<typename T>
	void load(std::unordered_map<std::string, T>& data){
		for (auto& m : internal.GetObject()){
			data[m.name.GetString()] = {};
			JSONParser j(internal[m.name.GetString()]);
			j.load(data[m.name.GetString()]);
		}
	}
	
	template<typename T>
	void load(std::vector<T>& data){
		for (auto& v : internal.GetArray()){
			data.emplace_back();
			JSONParser j(v);
			j.load(data.back());
		}
	}
	
	template<typename T, std::size_t N>
	void load(std::array<T,N>& data){
		for (int i = 0; i < data.size(); i++){
			JSONParser j(internal[i]);
			j.load(data[i]);
		}
	}

	template<typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
	void load(T& data){
		if constexpr (std::is_same<T, bool>::value){
			data = internal.GetBool();
		} else {
			data = internal.GetInt64();
		}
	}
	
	template<typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
	void load(T& data){
		data = internal.GetDouble();
	}

	template<typename T, std::enable_if_t<std::is_same<T, std::string>::value, int> = 0>
	void load(T& data){
		data = internal.GetString();
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
