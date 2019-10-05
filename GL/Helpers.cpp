#include "Helpers.hpp"
#include <fstream>
std::string readWholeFile(const std::string& filename){
	std::ifstream infile(filename,std::ios::in | std::ios::binary | std::ios::ate);
	std::string contents;
	if (infile){
		int size = infile.tellg();
		contents.resize(size);
		infile.seekg(0, std::ios::beg);
		infile.read(&contents[0],size);
		infile.close();
	}
	return contents;
}
template<typename T>
void genIndexBuffer(T max_sprites, GLBuffer<T>& buffer){
	static_assert(std::is_integral<T>::value, "Integral type required");
	max_sprites -= (max_sprites & 0x03);
	buffer.data.reserve((max_sprites*3 + 1)/2);
	size_t index = 0;
	for (T sprite = 0; sprite < max_sprites; sprite+=4){
		buffer.data.emplace_back(sprite);
		buffer.data.emplace_back(sprite+1);
		buffer.data.emplace_back(sprite+2);
		buffer.data.emplace_back(sprite+2);
		buffer.data.emplace_back(sprite+3);
		buffer.data.emplace_back(sprite);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer.data.size()*sizeof(T), buffer.data.data(), GL_STATIC_DRAW);
}
template void genIndexBuffer<uint16_t>(uint16_t, GLBuffer<uint16_t>&);