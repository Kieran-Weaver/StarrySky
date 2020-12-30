#ifndef STARRYSKY_RTREE_H
#define STARRYSKY_RTREE_H
#include <vector>
#include <array>
#include <ostream>
#include <util/Rect.hpp>
/*
 * 2D R-Tree Implementation
 * The algorithms are combinations of many different R-Tree algorithms:
 * 	- The bulk loading algorithm is OMT (Overlap Minimizing Top-down Bulk Loading Algorithm for R-Tree, from CAiSE Short Paper Proceedings)
 *  - The insertion and deletion algorithms are from the R*-Tree
 * TODO:
 *  - Implement OMT - Done
 *  - Implement Intersection/Collision - Done
 *  - Implement Insertion - Done
 *  - Implement Removal - Further research needed
 *  - Implement Replacement - Further research needed
 *  - Optimize finished R-Tree implementation - Not Started
 */
#define RT_OVERLAP_P  32u
#define RT_ROOT_NODE  0
#define RT_SMALL_M    0.4f
#define RT_REINSERT_P 0.3f

template<typename T = float>
struct RNode{
	RNode(size_t _M = 0) {
		children.resize(_M);
	}
	size_t size() const {
		return children.size();
	}
	size_t level = 0;
	Rect<T> AABB = {};
	std::vector<size_t> children;
};

template<typename T = float>
struct RLeaf{
	int id = 0;
	Rect<T> AABB = {};
};

struct RIndex{
	size_t idx = 0;
	bool leaf = false;
};

template<typename T>
class RTree{
public:
	RTree(size_t _M = 20) : M(_M) {
		this->m_nodes.emplace_back(RNode<T>(0));
	};
	void print(std::ostream& os);
	size_t size() const{
		return m_elements.size();
	}
	std::vector<int> intersect(const Rect<T>& object);
	std::vector<int> load(const std::vector<Rect<T>>& elements);
	int     insert(const Rect<T>& object);
	size_t  height = 0;
private:
	void    omt(int subroot, size_t N, size_t H, std::vector<size_t>::iterator& iter);
	void    printNode(size_t node, std::ostream& os);
	T       overlapCost(size_t idx, const Rect<T>& object);
	T       areaCost(size_t idx, const Rect<T>& object);
	std::vector<size_t> chooseSubTree(size_t idx, bool leaf);
	void    insertNode(size_t id, bool leaf, bool first = true);
	void    reinsert(size_t node);
	size_t  split(size_t nodeIdx);
	Rect<T> makeBound(std::vector<size_t>::const_iterator start, std::vector<size_t>::const_iterator end, bool leaves);
	int     getLevel(size_t idx, bool leaf);
	Rect<T> getAABB(size_t idx, bool leaf);
	size_t  M;
	std::vector<RNode<T>> m_nodes = {RNode<T>(0)};
	std::vector<RLeaf<T>> m_elements = {};
};

using FloatRTree = RTree<float>;
using IntRTree = RTree<int>;
using U64RTree = RTree<uint64_t>;

extern template class RTree<float>;
extern template class RTree<int>;
extern template class RTree<uint64_t>;
#endif
