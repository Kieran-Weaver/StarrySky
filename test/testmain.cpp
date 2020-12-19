#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Catch is working", "[misc]") {
	REQUIRE(1 == 1);
}
