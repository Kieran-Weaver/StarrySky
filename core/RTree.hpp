#ifndef STARRYSKY_RTREE_H
#define STARRYSKY_RTREE_H
#include <vector>
#include <array>
#include <GL/Rect.hpp>
/*
 * 2D R-Tree Implementation
 * Requirements for T:
 * Has a function with signature
 *     Rect<Dim> getAABB();
 */
template<class T, size_t M, typename Dim = float>
class RTree{
public:
	RTree() = default;
	RTree(const std::vector<T>& elements); // Builds RTree using OMT Algorithm
	~RTree();
	void print();
	void insert(const T& element);
	void remove(const T& element);
	size_t size() const{
		return m_elements.size();
	}
	std::vector<T> intersect(const T& object);
	size_t height = 0;
private:
	struct RNode{
		RNode* parent = nullptr;
		RNode* children = nullptr;
		bool leaf = false;
		int level = 0;
		int size = 0;
		Rect<Dim> AABB;
		std::vector<size_t> element_indices;
	};
	RNode root;
	void omt(RNode* subroot, size_t N, size_t H, std::vector<size_t>::iterator& iter);
	std::vector<T> m_elements;
	void printNode(RNode* node);
	void clear_rnode(RNode* node);
};
#endif
