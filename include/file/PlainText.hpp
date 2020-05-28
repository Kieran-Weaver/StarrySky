#ifndef STARRYSKY_HELPER_HPP
#define STARRYSKY_HELPER_HPP
#include <string>
#include <string_view>
std::string readWholeFile(const std::string& filename);
// Copy-on-write, does not save written data to file
class MMAPFile{
public:
	MMAPFile(const std::string& filename);
	~MMAPFile();
	std::string_view getString();
private:
	void* mapping = nullptr;
	uint64_t size = 0;
	void* fd;
};
#endif
