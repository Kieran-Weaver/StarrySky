#include <util/Mat2D.hpp>
glm::mat2 RotMat(const float& angle) {
	return glm::mat2(std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle));
}
glm::mat2 ScaleMat(const float& x, const float& y) {
	return glm::mat2(x,0,0,y);
}
glm::mat2 ShearMat(const float& x, const float& y) {
	return glm::mat2(1,x,y,1);
}
float angle(const glm::mat2& matrix) {
	return std::atan2(matrix[1][0], matrix[1][1]);
}
glm::vec2 scale(const glm::mat2& matrix) {
	return glm::vec2(matrix[0][0],matrix[1][1]);
}
glm::vec2 shear(const glm::mat2& matrix) {
	return glm::vec2(matrix[0][1]/matrix[0][0], matrix[1][0]/matrix[1][1]);
}
std::array<float, 5> decomp(const glm::mat2& matrix){
	const glm::mat2 temp(matrix[1][1], -matrix[1][0], -matrix[0][1], matrix[0][0]);
	const glm::mat2 q = matrix + temp;
	const glm::mat2 qs = q / (std::sqrt(q[0][0]*q[0][0] + q[1][0]*q[1][0]));
	const glm::mat2 s = transpose(qs) * matrix;
	const glm::vec2 sc = scale(s);
	const glm::vec2 sh = shear(s);
	return std::array<float,5>({angle(qs), sc[0], sc[1], sh[0], sh[1]});
}
glm::mat2 recompose(const std::array<float,5>& data){
	const auto rmat = RotMat(data[0]);
	const auto smat = ScaleMat(data[1], data[2]);
	const auto hmat = ShearMat(data[3], data[4]);
	return rmat * hmat * smat;
}
std::array<float,4> packmat2(const glm::mat2& matrix){
	return {matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]};
}
glm::mat2 unpackmat2(const std::array<float,4>& array){
	return {array[0], array[1], array[2], array[3]};
}
