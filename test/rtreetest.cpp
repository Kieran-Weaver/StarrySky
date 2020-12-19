#include <core/RTree.hpp>
#include <util/PRNG.hpp>
#include <catch2/catch.hpp>
#include <cassert>

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

TEST_CASE("Collision Test", "[RTree]") {
	std::mt19937 rng = SeedRNG();
	std::vector<AABBWrapper> collision_vec;
	std::vector<AABBWrapper> collided;
	std::vector<Rect<uint64_t>> aabbs;
	std::unordered_map<int, Rect<uint64_t>> elements;
	U64RTree tree(20);
	AABBWrapper ab = {randomRect(rng)};

	for (int i = 0; i < 5000; i++){
		aabbs.emplace_back(randomRect(rng));
	}

	const auto& indices = tree.load(aabbs);
	for (size_t i = 0; i < indices.size(); i++) {
		elements[indices[i]] = aabbs[i];
	}

	auto _cvec = tree.intersect(ab.internal);
	for (auto& idx : _cvec) {
		collision_vec.emplace_back(AABBWrapper({elements[idx]}));
	}
	std::sort(collision_vec.begin(), collision_vec.end());

	for (auto& vec : aabbs){
		if (ab.getAABB().Intersects(vec)){
			collided.emplace_back(AABBWrapper({vec}));
		}
	}
	std::sort(collided.begin(), collided.end());

	REQUIRE(collided == collision_vec);
}
