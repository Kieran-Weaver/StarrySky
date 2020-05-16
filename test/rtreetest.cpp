#include <core/RTree.hpp>
#include <util/PRNG.hpp>
#include <core/RTree.cpp>
#include <cassert>
struct AABBWrapper{
	Rect<uint64_t> internal;
	Rect<uint64_t> getAABB() const{
		return internal;
	}
	bool operator<(const AABBWrapper& other) const {
		return std::tie(internal.left, internal.top, internal.width, internal.height) < std::tie(other.internal.left, other.internal.top, other.internal.width, other.internal.height);
	}
	bool operator==(const AABBWrapper& other) const {
		return std::tie(internal.left, internal.top, internal.width, internal.height) == std::tie(other.internal.left, other.internal.top, other.internal.width, other.internal.height);
	}
};
bool refsort(const std::reference_wrapper<AABBWrapper>& a, const std::reference_wrapper<AABBWrapper>& b){
	return a.get() < b.get();
}
Rect<uint64_t> randomRect(std::mt19937& rng){
	return {rng() & 0xFF, rng() & 0xFF, rng() & 0x1F, rng() & 0x1F};
}
int main(int argc, char **argv){
	(void)argc;
	(void)argv;
	std::mt19937 rng = SeedRNG();
	std::vector<AABBWrapper> elements;
	for (int i = 0; i < 5000; i++){
		elements.emplace_back(AABBWrapper({randomRect(rng)}));
	}
	RTree<AABBWrapper, 20, uint64_t> tree(elements);
	tree.print();
	AABBWrapper ab = {randomRect(rng)};
	auto collision_vec = tree.intersect(ab.internal);
	std::sort(collision_vec.begin(), collision_vec.end(), refsort);
	std::vector<AABBWrapper> collided;
	for (auto& vec : elements){
		if (ab.getAABB().Intersects(vec.getAABB())){
			collided.emplace_back(vec);
		}
	}
	std::sort(collided.begin(), collided.end());
	for (size_t i=0; i < collided.size(); i++){
		assert(collided[i] == collision_vec[i].get());
	}
	std::cout << "Collision test finished successfully" << std::endl;
	return 0;
}
