#ifndef STARRYSKY_JSONNODE_HPP
#define STARRYSKY_JSONNODE_HPP
enum JSONType{
	JSONBOOL,
	JSONINT,
	JSONDBL,
	JSONSTR,
	JSONINVL
};
class JSONNode{
public:
	bool nodeBool = false;
	int64_t nodeInt = 0;
	double nodeDouble = 0.0;
	std::string nodestring = "";
	std::string nodepointer = "/";
	JSONNode operator[](std::string index);
	JSONNode operator[](int index);
	JSONType Type = JSONNONE;
private:
	JSONParser* data;
}
#endif