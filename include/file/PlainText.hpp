#ifndef STARRYSKY_PLAINTEXT_HPP
#define STARRYSKY_PLAINTEXT_HPP
#include <string_view>
std::string readWholeFile(const std::string_view filename);
// Copy-on-write, does not save written data to file
class MMAPFile{
public:
	MMAPFile(const std::string_view filename);
	~MMAPFile();
	std::string_view getString();
private:
	void* mapping = nullptr;
	uint64_t size = 0;
	void* fd;
};
#endif
