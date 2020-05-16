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
	RTree(const RTree& other) = delete;
	RTree(RTree&& other){
		this->root = other->root;
		this->empty_elements = other->empty_elements;
		this->m_elements = other->m_elements;
		other->m_elements = {};
		other->root = RNode();
		this->height = other->height;
		other->height = 0;
	}
	~RTree();
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
		RNode* parent = nullptr;
		RNode* children = nullptr;
		size_t level = 0;
		size_t size = 0;
		Rect<Dim> AABB;
		std::vector<size_t> element_indices;
	};
	RNode root;
	void omt(RNode* subroot, size_t N, size_t H, std::vector<size_t>::iterator& iter);
	std::vector<size_t> empty_elements;
	std::vector<T> m_elements;
	void printNode(RNode* node);
	void clear_rnode(RNode* node);
};
#endif
