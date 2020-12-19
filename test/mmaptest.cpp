#include <file/PlainText.hpp>
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("Test MMAP", "[file]") {
	MMAPFile mmf("test/test.txt");
	MMAPFile mmf2("filethatobviouslydoesnotexist.txt");
	std::string expected = "This is a test\nLine 2\n";

	REQUIRE(mmf.getString() == expected);	
	REQUIRE(mmf2.getString() == "");
}
