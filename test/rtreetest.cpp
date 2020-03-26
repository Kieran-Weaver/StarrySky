#include <core/RTree.hpp>
#include <core/PRNG.hpp>
#include <core/RTree.cpp>
struct AABBWrapper{
	Rect<float> internal;
	Rect<float> getAABB() const{
		return internal;
	}
};
Rect<float> randomRect(std::mt19937& rng){
	return {rng() & 0xFF, rng() & 0xFF, rng() & 0xFF, rng() & 0xFF};
}
int main(int argc, char **argv){
	std::mt19937 rng = SeedRNG();
	std::vector<AABBWrapper> elements;
	for (int i = 0; i < 50; i++){
		elements.emplace_back(AABBWrapper({randomRect(rng)}));
	}
	RTree<AABBWrapper, 4, float> tree(elements);
	tree.print();
	return 0;
}
