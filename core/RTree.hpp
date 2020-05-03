#ifndef STARRYSKY_RTREE_H
#define STARRYSKY_RTREE_H
#include <vector>
#include <array>
#include <GL/Rect.hpp>
/*
 * 2D R-Tree Implementation
 * Requirements for T:
 * Has a function with signature
 *     Rect<Dim> getAABB() const;
 * The algorithms are combinations of many different R-Tree algorithms:
 * 	- The bulk loading algorithm is OMT (Overlap Minimizing Top-down Bulk Loading Algorithm for R-Tree, from CAiSE Short Paper Proceedings)
 *  - To find the optimal subtree for insertion, the R*-Tree algorithm is used
 *  - The overflow handling from Compact R-Trees is used
 *  - The splitting algorithm from the NR-Tree is used ("A new enhancement to the R-Tree node splitting")
 * TODO:
 *  - Implement OMT - Done
 *  - Implement Intersection/Collision - Done
 *  - Implement Insertion - In Progress
 *  - Implement Removal - Not Started
 *  - Implement Replacement - Not Started
 *  - Optimize finished R-Tree implementation - Not Started
 */
template<class T, size_t M, typename Dim = float>
class RTree{
public:
	RTree() = default;
	RTree(const std::vector<T>& elements);
	~RTree();
	void print();
	void insert(const T& element);
	void remove(const T& element);
	size_t size() const{
		return m_elements.size();
	}
	std::vector<std::reference_wrapper<T>> intersect(const T& object);
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
	void insertRecursive(RNode* node, size_t index, const Rect<Dim>& aabb);
	std::optional<RNode*> find(const T& object);
};
#endif
