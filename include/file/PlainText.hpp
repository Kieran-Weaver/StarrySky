#ifndef STARRYSKY_PLAINTEXT_HPP
#define STARRYSKY_PLAINTEXT_HPP
#include <string_view>
#include <cstring>
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
	void* fd = nullptr;
};

template<typename T>
uint32_t read(const char* data, T& t) {
	std::memcpy(&t, data, sizeof(T));
	return sizeof(T);
}

template<>
inline uint32_t read(const char* data, std::string_view& t) {
	t = std::string_view(reinterpret_cast<const char*>(data));
	return t.size() + 1;
}

#endif
