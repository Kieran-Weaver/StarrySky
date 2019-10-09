#include "JSONNode.hpp"
#include <string>
JSONNode* JSONNode::operator[](int index){
	return this->operator[](std::to_string(index));
}
JSONNode* JSONNode::operator[](std::string index);
	std::string newpointer = this->nodepointer + "/" + index;
	return data->getNode(newpointer);
}