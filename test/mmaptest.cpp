#include <file/PlainText.hpp>
#include <iostream>
int main(int argc, char **argv){
	if (argc < 1){
		std::cout << "USAGE: ./mmaptest FILE" << std::endl;
	} else {
		MMAPFile mmf(argv[1]);
		std::cout << mmf.getString() << std::endl;
	}
}
