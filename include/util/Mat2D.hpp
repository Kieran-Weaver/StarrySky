#ifndef MAT2D_H
#define MAT2D_H
#include <glm/ext/matrix_float2x2.hpp>
#include <glm/ext/vector_float2.hpp>
#include <array>
glm::mat2 RotMat(const float& angle);
glm::mat2 ScaleMat(const float& x, const float& y);
glm::mat2 ShearMat(const float& x, const float& y);
float angle(const glm::mat2& matrix);
glm::vec2 scale(const glm::mat2& matrix);
glm::vec2 shear(const glm::mat2& matrix);
std::array<float, 5> decomp(const glm::mat2& matrix);
glm::mat2 recompose(const std::array<float,5>& data);
std::array<float,4> packmat2(const glm::mat2& matrix);
glm::mat2 unpackmat2(const std::array<float,4>& array);
#endif
