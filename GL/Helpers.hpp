#ifndef STARRYSKY_HELPER_HPP
#define STARRYSKY_HELPER_HPP
#include <string>
#include <vector>
std::string readWholeFile(const std::string& filename);
struct GLRect2D{
	uint16_t texRect[4];
	float packedMat[4];
	float sprPos[2];
};
#endif
