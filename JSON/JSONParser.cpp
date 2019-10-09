#include "JSONParser.hpp"
#include "JSONParserImpl.hpp"
JSONParser::JSONParser(std::string filename) : m_pImpl(new JSONParserImpl(filename)){}
JSONParser::JSONParser(SpriteBatch &&) noexcept = default;
JSONParser& JSONParser::JSONParser::operator=(JSONParser &&) noexcept = default;
JSONNode* JSONParser::getNode(const std::string& pointer){
	return pImpl->getNode(pointer);
}
void JSONParserImplDeleter::operator()(JSONParserImpl *p){
	delete p;
}