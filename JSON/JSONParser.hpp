#ifndef STARRYSKY_JSONPARSER_HPP
#define STARRYSKY_JSONPARSER_HPP
class JSONParserImpl;
class JSONNode;
struct JSONParserImplDeleter{
	void operator()(JSONParserImpl *p);
};
class JSONParser{
public:
	explicit JSONParser(std::string filename);
	~JSONParser() = default;
	JSONParser(JSONParser &&) noexcept;
	JSONParser& operator=(JSONParser &&) noexcept;
	JSONParser(const JSONParser& rhs) = delete;
	JSONParser& operator=(const JSONParser& rhs) = delete;
	JSONNode * getNode(const std::string& pointer);
private:
	const JSONParserImpl* Pimpl() const { return m_pImpl.get(); }
	JSONParserImpl* Pimpl(){ return m_pImpl.get(); }
	std::unique_ptr<JSONParserImpl,JSONParserImplDeleter> m_pImpl;	
}
#endif