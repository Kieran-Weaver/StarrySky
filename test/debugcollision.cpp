/* Collision Debugger
 * Converts bin.gz files to plain PBMs (netpbm format, magic P1)
 */
#include <GL/TextureAtlas.hpp>
#include <iostream>
int main(int argc, char **argv){
	TextureAtlas atlas("data/atlas.json");
	Texture t = atlas.findSubTexture("test1");
	std::cout << "P1" << std::endl;
	std::cout << t.m_bitmask.width << " " << t.m_bitmask.height << std::endl;
	for (int j = 0; j < t.m_bitmask.height; j++){
		for (int i = 0; i < t.m_bitmask.width; i++){
			std::cout << t.m_bitmask.get(i, j) << " ";
		}
		std::cout << std::endl;
	}
}
