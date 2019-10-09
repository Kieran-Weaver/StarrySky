#ifndef STARRYSKY_JSONPARSERIMPL_HPP
#define STARRYSKY_JSONPARSERIMPL_HPP
#include <simdjson/jsonparser.h>
class JSONNode;
class JSONParserImpl{
public:
	JSONParserImpl(std::string filename);
	JSONNode * getNode(std::string pointer);
private:
	simdjson::ParsedJson pj;
	simdjson::ParsedJson::Iterator it;
	bool valid = false;
}
#endif