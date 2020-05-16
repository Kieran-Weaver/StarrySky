#include <util/Mat2D.hpp>
#include <util/PRNG.hpp>
#include <iostream>
void printMat2(const glm::mat2& m2){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2; j++){
			std::cout << m2[i][j] << std::endl;
		}
	}
}
int main(){
	auto rng = SeedRNG();
	glm::mat2 random_matrix(rng(), rng(), rng(), rng());
	printMat2(random_matrix);
	std::array<float,5> decomposed = decomp(random_matrix);
	glm::mat2 recomposed = recompose(decomposed);
	printMat2(recomposed);
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2; j++){
			assert(std::fabs(random_matrix[i][j] - recomposed[i][j]) < 0.01f*random_matrix[i][j]);
		}
	}
}
