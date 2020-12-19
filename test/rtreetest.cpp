#include <core/RTree.hpp>
#include <util/PRNG.hpp>
#include <cassert>
#include <iostream>
struct AABBWrapper{
	Rect<uint64_t> internal;
	Rect<uint64_t> getAABB() const{
		return internal;
	}
	bool operator<(const AABBWrapper& other) const {
		return std::tie(internal.left, internal.top, internal.right, internal.bottom) < std::tie(other.internal.left, other.internal.top, other.internal.right, other.internal.bottom);
	}
	bool operator==(const AABBWrapper& other) const {
		return std::tie(internal.left, internal.top, internal.right, internal.bottom) == std::tie(other.internal.left, other.internal.top, other.internal.right, other.internal.bottom);
	}
};
Rect<uint64_t> randomRect(std::mt19937& rng){
	uint64_t left = rng() & 0xFF;
	uint64_t top = rng() & 0xFF;
	uint64_t w = rng() & 0x1F;
	uint64_t h = rng() & 0x1F;
	return {left, top, left + w, top + h};
}
int main(int argc, char **argv){
	(void)argc;
	(void)argv;
	std::mt19937 rng = SeedRNG();
	std::vector<AABBWrapper> collision_vec;
	std::vector<Rect<uint64_t>> aabbs;
	std::unordered_map<int, Rect<uint64_t>> elements;
	for (int i = 0; i < 5000; i++){
		aabbs.emplace_back(randomRect(rng));
	}
	U64RTree tree(20);
	const auto& indices = tree.load(aabbs);
	for (size_t i = 0; i < indices.size(); i++) {
		elements[indices[i]] = aabbs[i];
	}
	
	tree.print();

	AABBWrapper ab = {randomRect(rng)};
	auto _cvec = tree.intersect(ab.internal);
	for (auto& idx : _cvec) {
		collision_vec.emplace_back(AABBWrapper({elements[idx]}));
	}
	std::sort(collision_vec.begin(), collision_vec.end());

	std::vector<AABBWrapper> collided;
	for (auto& vec : aabbs){
		if (ab.getAABB().Intersects(vec)){
			collided.emplace_back(AABBWrapper({vec}));
		}
	}
	std::sort(collided.begin(), collided.end());

	assert(collided.size() == collision_vec.size());
	for (size_t i=0; i < collided.size(); i++){
		assert(collided[i] == collision_vec[i]);
	}
	std::cout << "Collision test finished successfully" << std::endl;
	return 0;
}
