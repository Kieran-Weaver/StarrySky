#include <string>
#include <rapidjson/document.h>
#include <glm/glm.hpp>
#include <file/JSONReader.hpp>

enum TMType{
	Normal,
	Effect
};

JSONParser JSONParser::operator[](const char* key) const {
	return {internal[key]};
}

JSONParser JSONParser::operator[](std::string_view key) const {
	return {internal[key.data()]};
}

JSONParser JSONParser::operator[](int key) const {
	return {internal[key]};
}

bool JSONParser::HasKey(const std::string& key) const{
	return internal.HasMember(key.c_str());
}

bool JSONParser::IsString() const{
	return internal.IsString();
}

std::vector<std::string_view> JSONParser::getKeys() const{
	std::vector<std::string_view> keys;
	for (auto& m : internal.GetObject()){
		keys.emplace_back(m.name.GetString());
	}
	return keys;
}

int JSONParser::size() const{
	return internal.Size();
}

JSONParser::operator int64_t() const {
	if (internal.IsBool()){
		return internal.GetBool();
	} else {
		return internal.GetInt64();
	}
}

JSONParser::operator double() const {
	return internal.GetDouble();
}

JSONParser::operator std::string() const {
	return internal.GetString();
}

JSONReader::JSONReader(const char* text){
	document.reset(new rapidjson::Document);
	document->Parse(text);
}

JSONParser JSONReader::operator[](const std::string& key){
	return {(*document)[key.c_str()]};
}

void DocDeleter::operator()(rapidjson::Document* doc){
	delete doc;
}

template<>
JSONParser::operator glm::mat2() const{
	glm::mat2 data;
	data[0][0] = internal[0].GetFloat();
	data[0][1] = internal[1].GetFloat();
	data[1][0] = internal[2].GetFloat();
	data[1][1] = internal[3].GetFloat();
	return data;
}

template<>
JSONParser::operator TMType() const{
	std::string data{internal.GetString()};
	if (data == "normal"){
		return TMType::Normal;
	} else if (data == "effect"){
		return TMType::Effect;
	} else {
		return TMType::Normal;
	}
}
