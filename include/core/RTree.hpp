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
template<class T, size_t M, typename Dim = float>
class RTree{
public:
	RTree() = default;
	RTree(const std::vector<T>& elements);
	void print();
	size_t size() const{
		return m_elements.size();
	}
	std::vector<std::reference_wrapper<T>> intersect(const Rect<Dim>& object);
	void load(const std::vector<T>& elements);
	const std::vector<T>& get_elements(){
		return m_elements;
	}
	size_t height = 0;
private:
	struct RNode{
		int parent = -1;
		size_t level = 0;
		size_t size = 0;
		std::array<size_t, M> children;
		Rect<Dim> AABB;
	};
	void omt(int subroot, size_t N, size_t H, std::vector<size_t>::iterator& iter);
	void printNode(int node);
	std::vector<RNode> m_nodes;
	std::vector<T> m_elements;
};
#endif
