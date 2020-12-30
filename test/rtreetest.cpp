#include <core/RTree.hpp>
#include <util/PRNG.hpp>
#include <catch2/catch.hpp>
#define STRESS_N 65536

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

	for (int i = 0; i < STRESS_N; i++){
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

TEST_CASE("Test Insert Without Split or Reinsert", "[RTree]") {
	U64RTree tree(20);
	std::array<Rect<uint64_t>, 4> boxes = {{
		{0, 0, 4, 4}, {1, 1, 4, 6}, {5, 5, 7, 8}, {10, 0, 15, 4}
	}};
	Rect<uint64_t> disjointBox = {50, 50, 60, 60};
	Rect<uint64_t> boundingBox = {0, 0, 20, 20};
	std::unordered_map<int, size_t> IDtoArrIndex = {};
	
	for (size_t i = 0; i < boxes.size(); i++) {
		int id = tree.insert(boxes[i]);
		REQUIRE(IDtoArrIndex.count(id) == 0);
		IDtoArrIndex[id] = i;
	}
	
	REQUIRE(tree.intersect(disjointBox).empty());
	auto collided = tree.intersect(boundingBox);
	
	for (auto& id : collided) {
		REQUIRE(IDtoArrIndex.count(id) == 1);
		REQUIRE(boxes[IDtoArrIndex[id]].Intersects(boundingBox));
	}
}

TEST_CASE("Test Insert with Split and Reinsert", "[RTree]") {
	U64RTree tree(20);
	std::array<Rect<uint64_t>, 25> boxes = {{
		{0, 0, 4, 4},     {1, 1, 4, 6},     {5, 5, 7, 8},     {10, 0, 15, 4},
		{0, 10, 15, 14},  {12, 4, 16, 8},   {50, 0, 54, 6},   {3, 1, 9, 5},
		{15, 3, 20, 4},   {10, 0, 15, 4},   {49, 64, 57, 76}, {57, 27, 71, 40},
		{59, 11, 72, 24}, {33, 7, 44, 11},  {12, 60, 21, 76}, {60, 22, 61, 33},
		{55, 63, 58, 67}, {39, 47, 48, 63}, {60, 25, 67, 33}, {25, 51, 41, 60},
		{61, 1, 71, 15},  {5, 28, 6, 37},   {18, 54, 21, 66}, 
		{9, 38, 15, 54},  {53, 39, 59, 47}
	}};
	Rect<uint64_t> disjointBox = {99, 99, 100, 100};
	Rect<uint64_t> boundingBox = {0, 0, 100, 100};
	std::unordered_map<int, size_t> IDtoArrIndex = {};
	bool uniqueIDs = true;
	
	for (size_t i = 0; (i < boxes.size()) && uniqueIDs; i++) {
		int id = tree.insert(boxes[i]);
		uniqueIDs = (IDtoArrIndex.count(id) == 0);
		IDtoArrIndex[id] = i;
	}
	
	REQUIRE(uniqueIDs);
	
	REQUIRE(tree.intersect(disjointBox).empty());
	auto collided = tree.intersect(boundingBox);
	
	for (auto& id : collided) {
		REQUIRE(IDtoArrIndex.count(id) == 1);
		REQUIRE(boxes[IDtoArrIndex[id]].Intersects(boundingBox));
	}
}

TEST_CASE("Insert Stress Test", "[RTree]") {
	std::mt19937 rng = SeedRNG();
	std::vector<AABBWrapper> collision_vec;
	std::vector<AABBWrapper> collided;
	std::vector<Rect<uint64_t>> aabbs;
	std::unordered_map<int, Rect<uint64_t>> elements;
	U64RTree tree(20);
	AABBWrapper ab = {randomRect(rng)};

	for (int i = 0; i < STRESS_N; i++){
		aabbs.emplace_back(randomRect(rng));
	}

	for (size_t i = 0; i < aabbs.size(); i++) {
		int id = tree.insert(aabbs[i]);
		elements[id] = aabbs[i];
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
