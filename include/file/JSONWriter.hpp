#ifndef STARRYSKY_JSONWRITER_HPP
#define STARRYSKY_JSONWRITER_HPP
#include <rapidjson/fwd.h>
#include <visit_struct/visit_struct.hpp>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <util/Traits.hpp>
using StringBuffer_t = rapidjson::StringBuffer;
using Writer_t = rapidjson::Writer<rapidjson::StringBuffer, rapidjson::UTF8<char>, rapidjson::UTF8<char>, rapidjson::CrtAllocator, 0>;
struct SBDeleter{
	void operator()(StringBuffer_t*);
};

struct WrDeleter{
	void operator()(Writer_t*);
};

class JSONWriter{
public:
	explicit JSONWriter();
	void Key(const std::string& name);
	void StartArray();
	void EndArray();
	void StartObject();
	void EndObject();
	void store(bool data);
	void store(int64_t data);
	void store(double data);
	void store(std::string_view data);
	std::string_view GetString();

	template<typename T2, std::enable_if_t< \
	!(visit_struct::traits::is_visitable<T2>::value || is_json_literal<T2>() || is_std_array<T2>::value) \
	, int> = 0>
	void store(const T2& data);
	
	template<typename T2, std::enable_if_t<is_json_literal<T2>(), int> = 0>
	void store(const T2& data){
		if constexpr (std::is_same_v<T2, bool>) {
			this->store(static_cast<bool>(data));
		} else if constexpr (std::is_integral_v<T2>) {
			this->store(static_cast<int64_t>(data));
		} else if constexpr (std::is_floating_point_v<T2>) {
			this->store(static_cast<double>(data));
		} else if constexpr (std::is_convertible_v<T2, std::string_view>) {
			this->store(static_cast<std::string_view>(data));
		}
	}
	
	template<typename T2, std::enable_if_t<visit_struct::traits::is_visitable<T2>::value, int> = 0>
	void store(const T2& data){
		this->StartObject();
		visit_struct::for_each(data, [&](const char* name, const auto& value){
			this->Key(name);
			this->store(value);
		});
		this->EndObject();
	}

	template<typename T2>
	void store(const std::unordered_map<std::string,T2>& data){
		this->StartObject();
		for (const auto& elem : data){
			this->Key(elem.first);
			this->store(elem.second);
		}
		this->EndObject();
	}

	template<typename T2>
	void store(const std::unordered_map<int,T2>& data){
		this->StartArray();
		for (const auto& elem : data){
			this->store(elem.second);
		}
		this->EndArray();
	}

	template<typename T, std::size_t N>
	void store(const std::array<T,N>& data){
		this->StartArray();
		for (const auto &elem : data){
			this->store(elem);
		}
		this->EndArray();
	}

	template<typename T2>
	void store(const std::vector<T2>& data){
		this->StartArray();
		for (const auto& elem : data){
			this->store(elem);
		}
		this->EndArray();
	}

private:
	std::unique_ptr<StringBuffer_t, SBDeleter> buffer;
	std::unique_ptr<Writer_t, WrDeleter> writer;
};
#endif
