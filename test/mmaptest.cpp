#include <file/PlainText.hpp>
#include <iostream>
int main(int argc, char **argv){
	if (argc < 2){
		MMAPFile mmf(__FILE__);
		std::cout << mmf.getString() << std::endl;
	} else {
		MMAPFile mmf(argv[1]);
		std::cout << mmf.getString() << std::endl;
	}
}
