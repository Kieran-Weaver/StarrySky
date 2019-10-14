#ifndef STARRYSKY_JSON_HELPER_HPP
#define STARRYSKY_JSON_HELPER_HPP
#include <sajson.h>
inline std::string get_string(const sajson::value& node, std::string key){
	return node.get_value_of_key(sajson::string(key.c_str(), key.length())).as_string();
}
inline int get_int(const sajson::value& node, std::string key){
	return node.get_value_of_key(sajson::string(key.c_str(), key.length())).get_integer_value();
}
inline bool get_bool(const sajson::value& node, std::string key){
	return (node.get_value_of_key(sajson::string(key.c_str(), key.length())).get_type() == sajson::TYPE_TRUE);
}
inline sajson::value get_node(const sajson::value& node, std::string key){
	return node.get_value_of_key(sajson::string(key.c_str(), key.length()));
}
#endif
