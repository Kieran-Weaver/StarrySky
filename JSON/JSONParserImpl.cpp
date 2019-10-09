#include "JSONNode.hpp"
#include "JSONParserImpl.hpp"
JSONParserImpl::JSONParserImpl(std::string filename){
	this->pointer = "";
	simdjson::padded_string p = simdjson::get_corpus(filename);
	pj = simdjson::build_parsed_json(p);
	if (pj.is_valid()){
		it = simdjson::ParsedJson::Iterator(pj);
		this->valid = true;
	}
}
JSONNode* JSONParserImpl::getNode(std::string pointer){
	JSONNode* newNode = nullptr;
	if (this->valid){
		it.move_to(pointer);
		newNode = new JSONNode;
		newNode.nodepointer = pointer;
		if (it.is_string()){
			newNode.nodestring = it.get_string();
			newNode.Type = JSONType::JSONSTR;
		}else if (it.is_true() || it.is_false()){
			newNode.nodebool = it.is_true();
			newNode.Type = JSONType::JSONBOOL;
		}else if (it.is_integer()){
			newNode.nodeInt = it.get_integer();
			newNode.Type = JSONType::JSONINT;
		}else if (it.is_double()){
			newNode.nodeDouble = it.get_double();
			newNode.Type = JSONType::JSONDBL;
		}else{
			newNode.Type = JSONType::JSONINVL;
		}
	}
	return newNode;
}