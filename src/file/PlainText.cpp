#include <file/PlainText.hpp>
#include <fstream>
std::string readWholeFile(const std::string_view filename){
	std::ifstream infile(filename.data(),std::ios::in | std::ios::binary | std::ios::ate);
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
std::string_view MMAPFile::getString(){
	if (this->size) {
		return std::string_view(reinterpret_cast<char*>(this->mapping), this->size);
	} else {
		return "";
	}
}
#if defined _WIN32 || defined __CYGWIN__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
MMAPFile::MMAPFile(const std::string_view filename){
	WIN32_FILE_ATTRIBUTE_DATA file_attr_data;
	LARGE_INTEGER file_size = {0};
	if (GetFileAttributesEx(filename.data(), GetFileExInfoStandard, &file_attr_data)){
		file_size.HighPart = file_attr_data.nFileSizeHigh;
		file_size.LowPart = file_attr_data.nFileSizeLow;

		this->fd = CreateFile(filename.data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE mhandle = CreateFileMapping(fd, 0, PAGE_WRITECOPY, 0, 0, 0);
		this->mapping = MapViewOfFile(mhandle, FILE_MAP_COPY, 0, 0, 0);
		CloseHandle(mhandle);
	}
	this->size = file_size.QuadPart;	
}
MMAPFile::~MMAPFile(){
	UnmapViewOfFile(this->mapping);
	CloseHandle(this->fd);
}
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
MMAPFile::MMAPFile(const std::string_view filename){
	struct stat st;
	if (stat(filename.data(), &st) == 0) {
		this->size = st.st_size;
		this->fd = new int;
		int* fdp = reinterpret_cast<int*>(fd);
		*fdp = open(filename.data(), O_RDONLY);
		this->mapping = mmap(nullptr, this->size, PROT_READ | PROT_WRITE, MAP_PRIVATE, *fdp, 0);
	} else {
		this->mapping = nullptr;
		this->fd = nullptr;
		this->size = 0;
	}
}
MMAPFile::~MMAPFile(){
	if (this->mapping) {
		munmap(this->mapping, this->size);
		int* fdp = reinterpret_cast<int*>(fd);
		close(*fdp);
		delete fdp;
	}
}
#endif
