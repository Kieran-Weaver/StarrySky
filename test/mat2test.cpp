#define _USE_MATH_DEFINES
#include <catch2/catch.hpp>
#include <util/Mat2D.hpp>
#include <cmath>
#define RSQRT_2 (1.f / std::sqrt(2))

bool checkMat2(const glm::mat2& testcase, const glm::mat2& expected) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			if (expected[i][j] == 0.f) {
				if (testcase[i][j] != Approx(expected[i][j]).margin(1e-5)) {
					return false;
				}
			} else if (testcase[i][j] != Approx(expected[i][j]).epsilon(1e-5)) {
				return false;
			}
		}
	}
	return true;
}

template<typename T, size_t N>
bool checkArrayApprox(const std::array<T, N>& testcase, const std::array<T, N>& expected) {
	for (size_t i = 0; i < N; i++) {
		if (expected[i] == 0.f) {
			if (testcase[i] != Approx(expected[i]).margin(1e-5)) {
				return false;
			}
		} else if (testcase[i] != Approx(expected[i])) {
			return false;
		}
	}
	return true;
}

TEST_CASE("Test packmat2 and unpackmat2", "[matrix]") {
	const std::array<float, 4> testarr = {1.f, 2.f, 3.f, 4.f};
	const glm::mat2 testmat = {1.f, 2.f, 3.f, 4.f};

	// Packing and unpacking {} should preserve all 0s
	REQUIRE(packmat2({}) == std::array<float, 4>());
	REQUIRE(unpackmat2({}) == glm::mat2());
	
	// Packing and unpacking values should work in the correct order
	REQUIRE(packmat2(testmat) == testarr);
	REQUIRE(unpackmat2(testarr) == testmat);
	
	// Pack and unpack should be inverses
	REQUIRE(packmat2(unpackmat2(testarr)) == testarr);
	REQUIRE(unpackmat2(packmat2(testmat)) == testmat);
}

TEST_CASE("Test RotMat and angle", "[matrix]") {
	// A rotation of 0 is the identity matrix
	REQUIRE(RotMat(0.f) == glm::mat2(1.f, 0.f, 0.f, 1.f));
	REQUIRE(angle({1.f, 0.f, 0.f, 1.f}) == 0.f);
	
	// Rotation matrices with angles between 0 and 2*PI
	REQUIRE(checkMat2(RotMat(M_PI_2), {0.f, -1.f, 1.f, 0.f}));
	REQUIRE(angle({0.f, -1, 1, 0.f}) == Approx(M_PI_2));
	
	REQUIRE(checkMat2(RotMat(M_PI_4), {RSQRT_2, -RSQRT_2, RSQRT_2, RSQRT_2}));
	REQUIRE(angle({sqrt(2.f), -sqrt(2.f), sqrt(2.f), sqrt(2.f)}) == Approx(M_PI_4));
	
	// Rotation matrix with negative angle
	REQUIRE(checkMat2(RotMat(-M_PI_2), {0.f, 1.f, -1.f, 0.f}));
	REQUIRE(angle({0.f, 1.f, -1.f, 0.f}) == Approx(-M_PI_2));
	
	// Rotation matrix with angle larger than 2*PI
	REQUIRE(checkMat2(RotMat((2 * M_PI) + M_PI_2), glm::mat2(0.f, -1.f, 1.f, 0.f)));
}

TEST_CASE("Test ScaleMat and scale", "[matrix]") {
	// Scale 1 is the identity matrix
	REQUIRE(ScaleMat(1.f, 1.f) == glm::mat2(1.f, 0.f, 0.f, 1.f));
	REQUIRE(scale({1.f, 0.f, 0.f, 1.f}) == glm::vec2(1.f, 1.f));
	
	// Scale 0 is the zero matrix
	REQUIRE(ScaleMat(0.f, 0.f) == glm::mat2(0.f, 0.f, 0.f, 0.f));
	REQUIRE(scale({0.f, 0.f, 0.f, 0.f}) == glm::vec2(0.f, 0.f));
	
	// Positive scaling values
	REQUIRE(ScaleMat(2.f, 3.f) == glm::mat2(2.f, 0.f, 0.f, 3.f));
	REQUIRE(scale({2.f, 0.f, 0.f, 3.f}) == glm::vec2(2.f, 3.f));

	// Negative scaling values
	REQUIRE(ScaleMat(-4.f, -0.1f) == glm::mat2(-4.f, 0.f, 0.f, -0.1f));
	REQUIRE(scale({-4.f, 0.f, 0.f, -0.1f}) == glm::vec2(-4.f, -0.1f));
}

TEST_CASE("Test ShearMat and shear", "[matrix]") {
	// Shear of 0 is the identity matrix
	REQUIRE(ShearMat(0.f, 0.f) == glm::mat2(1.f, 0.f, 0.f, 1.f));
	REQUIRE(shear({1.f, 0.f, 0.f, 1.f}) == glm::vec2(0.f, 0.f));
	
	// Positive shear values
	REQUIRE(ShearMat(2.f, 3.f) == glm::mat2(1.f, 2.f, 3.f, 1.f));
	REQUIRE(shear({1.f, 2.f, 3.f, 1.f}) == glm::vec2(2.f, 3.f));
	
	// Negative shear values
	REQUIRE(ShearMat(-4.f, -0.1f) == glm::mat2(1.f, -4.f, -0.1f, 1.f));
	REQUIRE(shear({1.f, -4.f, -0.1f, 1.f}) == glm::vec2(-4.f, -0.1f));
}

TEST_CASE("Test decomp and recompose", "[matrix]") {
	const glm::mat2 rm = RotMat(1.f);
	const glm::mat2 sm = ScaleMat(2.f, -0.4f);
	const glm::mat2 qm = ShearMat(-3.f, 4.2f);
	const glm::mat2 combined = rm * sm * qm;

	// Primitive rotation and scaling matrices
	REQUIRE(checkArrayApprox(decomp(rm), std::array<float, 5>({1.f, 1.f,  1.f,  0.f, 0.f})));
	REQUIRE(checkArrayApprox(decomp(sm), std::array<float, 5>({0.f, 2.f, -0.4f, 0.f, 0.f})));

	// Recompose should produce rm, sm, qm
	REQUIRE(checkMat2(recompose({1.f, 1.f,  1.f,  0.f, 0.f}), rm));
	REQUIRE(checkMat2(recompose({0.f, 2.f, -0.4f, 0.f, 0.f}), sm));
	REQUIRE(checkMat2(recompose({0.f, 1.f,  1.f, -3.f, 4.2f}), qm));
	
	// Inverse operations
	REQUIRE(checkMat2(recompose(decomp(combined)), combined));
	REQUIRE(checkArrayApprox(decomp(recompose(decomp(combined))), decomp(combined)));
}
