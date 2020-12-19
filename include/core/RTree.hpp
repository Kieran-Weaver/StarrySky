#ifndef STARRYSKY_RTREE_H
#define STARRYSKY_RTREE_H
#include <vector>
#include <array>
#include <util/Rect.hpp>
/*
 * 2D R-Tree Implementation
 * Requirements for T:
 * Has a function with signature
 *     Rect<Dim> getAABB() const;
 * The algorithms are combinations of many different R-Tree algorithms:
 * 	- The bulk loading algorithm is OMT (Overlap Minimizing Top-down Bulk Loading Algorithm for R-Tree, from CAiSE Short Paper Proceedings)
 * TODO:
 *  - Implement OMT - Done
 *  - Implement Intersection/Collision - Done
 *  - Implement Insertion - Further research needed
 *  - Implement Removal - Further research needed
 *  - Implement Replacement - Further research needed
 *  - Optimize finished R-Tree implementation - Not Started
 */
 
template<typename T = float>
struct RNode{
	RNode(size_t _M) {
		children.resize(_M);
	}
	size_t size() {
		return children.size();
	}
	int parent = -1;
	size_t level = 0;
	Rect<T> AABB = {};
	std::vector<size_t> children;
};

template<typename T = float>
struct RLeaf{
	int id = 0;
	Rect<T> AABB = {};
};

template<class T, typename Dim = float>
class RTree{
public:
	RTree() : M(20) {};
	RTree(size_t _M) : M(_M) {};
	void print();
	size_t size() const{
		return m_elements.size();
	}
	std::vector<int> intersect(const Rect<Dim>& object);
	std::vector<int> load(const std::vector<Rect<Dim>>& elements);
	size_t height = 0;
private:
	void omt(int subroot, size_t N, size_t H, std::vector<size_t>::iterator& iter);
	void printNode(int node);
	size_t M;
	std::vector<RNode<Dim>> m_nodes;
	std::vector<RLeaf<Dim>> m_elements;
};

#endif
