#include <GL/SpriteData.hpp>
#include <iostream>
// Prints out packing data for the SpriteData structure, useful for debugging resources/data/shaders.json
int main(){
	std::cout << "Sizeof SpriteData: " << sizeof(SpriteData) << std::endl;
	std::cout << "offsetof(texture uv): " << offsetof(SpriteData, texpos) << std::endl;
	std::cout << "offsetof(vertex position): " << offsetof(SpriteData, vtxPos) << std::endl;
	std::cout << "offsetof(color multiplier): " << offsetof(SpriteData, sprColor) << std::endl;
	return 0;
}
