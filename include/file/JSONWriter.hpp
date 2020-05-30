#ifndef STARRYSKY_JSONWRITER_HPP
#define STARRYSKY_JSONWRITER_HPP
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <visit_struct/visit_struct.hpp>
#include <util/Traits.hpp>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>
class JSONWriter{
public:
	JSONWriter() : writer(buffer){}
	void Key(const std::string& name){
		writer.Key(name.c_str());
	}
	void StartObject(){
		writer.StartObject();
	}
	void EndObject(){
		writer.EndObject();
	}
	
	template<typename T2, std::enable_if_t< \
	!(visit_struct::traits::is_visitable<T2>::value || is_json_literal<T2>() || is_std_array<T2>::value) \
	, int> = 0>
	void store(const T2& data);
	
	template<typename T2, std::enable_if_t<visit_struct::traits::is_visitable<T2>::value, int> = 0>
	void store(const T2& data){
		writer.StartObject();
		visit_struct::for_each(data, [&](const char* name, const auto& value){
			writer.Key(name);
			this->store(value);
		});
		writer.EndObject();
	}

	template<typename T2>
	void store(const std::unordered_map<std::string,T2>& data){
		writer.StartObject();
		for (const auto& elem : data){
			writer.Key(elem.first.c_str());
			this->store(elem.second);
		}
		writer.EndObject();
	}

	template<typename T2>
	void store(const std::unordered_map<int,T2>& data){
		writer.StartArray();
		for (const auto& elem : data){
			this->store(elem.second);
		}
		writer.EndArray();
	}
	
	template<typename T, std::size_t N>
	void store(const std::array<T,N>& data){
		writer.StartArray();
		for (const auto &elem : data){
			this->store(elem);
		}
		writer.EndArray();
	}
	
	template<typename T2, std::enable_if_t<std::is_integral<T2>::value, int> = 0>
	void store(const T2& data){
		if constexpr (std::is_same<T2, bool>::value){
			writer.Bool(data);
		} else {
			writer.Int64(data);
		}
	}

	template<typename T2, std::enable_if_t<std::is_floating_point<T2>::value, int> = 0>
	void store(const T2& data){
		writer.Double(data);
	}

	template<typename T2>
	void store(const std::vector<T2>& data){
		writer.StartArray();
		for (const auto& elem : data){
			this->store(elem);
		}
		writer.EndArray();
	}

	template<typename T2, std::enable_if_t<std::is_same<T2, std::string>::value, int> = 0>
	void store(const T2& data){
		writer.String(data.c_str());
	}

	std::string GetString(){
		return buffer.GetString();
	}
private:
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer;
};
#endif
