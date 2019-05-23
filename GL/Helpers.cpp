#include "Helpers.hpp"
std::string readWholeFile(const std::string& filename){
	std::ifstream infile(filename,std::ios::in | std::ios::binary | std::ios::ate);
	if (infile){
		std::string contents;
		int size = infile.tellg();
		contents.resize(size);
		infile.seekg(0, std::ios::beg);
		infile.read(&contents[0],size);
		infile.close();
		return contents;
	}else{
		return "";
	}
}
