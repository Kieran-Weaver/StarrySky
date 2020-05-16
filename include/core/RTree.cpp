#ifndef RTREE_IMPL
#define RTREE_IMPL
#include <core/RTree.hpp>
#include <util/RectCompare.hpp>
#include <numeric>
#include <cmath>
#include <iostream>
template<class T, size_t M, typename Dim>
RTree<T,M,Dim>::RTree(const std::vector<T>& elements){
	this->load(elements);
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::load(const std::vector<T>& elements){
	m_elements = elements;
	size_t N = m_elements.size();
	size_t H = std::ceil(std::log2(N) / std::log2(M));
	double N_subtree = std::pow(M,H-1);
	size_t S = std::floor(std::sqrt(std::ceil(N/N_subtree)));
	this->height = H;
	this->root.children = new RNode[S];
	this->root.level = H;
	std::sort(m_elements.begin(), m_elements.end(), [&](const T& lhs, const T& rhs){
		return rectCompare<Dim>(lhs.getAABB(),rhs.getAABB(), H & 0x01);
	});
	std::vector<size_t> tempVec;
	tempVec.resize(N);
	std::iota(tempVec.begin(), tempVec.end(), 0);
	auto iter = tempVec.begin();
	size_t i;
	std::vector<Rect<Dim>> aabbs;
	for (i = 0; i < (S-1); i++){
		root.children[i].parent = &root;
		this->omt(root.children + i, S, H-1, iter);
		aabbs.emplace_back(root.children[i].AABB);
	}
	size_t distance = tempVec.end() - iter;
	this->omt(root.children + i, distance, H-1, iter);
	aabbs.emplace_back(root.children[i].AABB);
	root.size = S;
	root.AABB = join<Dim>(aabbs.begin(), aabbs.end());
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::omt(RNode* subroot, size_t N, size_t level, std::vector<size_t>::iterator& iter){
	std::vector<Rect<Dim>> aabbs;
	if (N < M){
		RNode leaf = { nullptr, nullptr, 0, N, {}, {}};
		leaf.element_indices.reserve(M);
		aabbs.reserve(N);
		for (size_t i = 0; i < N; i++){
			leaf.element_indices.emplace_back(*iter);
			iter++;
			aabbs.emplace_back(m_elements[leaf.element_indices[i]].getAABB());
		}
		leaf.AABB = join<Dim>(aabbs.begin(), aabbs.end());
		while (level != 0){
			subroot->size = 1;
			subroot->level = level;
			subroot->children = new RNode[M];
			subroot->children[0].parent = subroot;
			subroot->AABB = leaf.AABB;
			level--;
			subroot = subroot->children;
		}
		*subroot = leaf;
	} else {
		subroot->level = level;
		std::sort(iter, iter + N, [&](const size_t& lhs, const size_t& rhs){
			return rectCompare<Dim>(m_elements[lhs].getAABB(),m_elements[rhs].getAABB(), level & 0x01);
		});
		size_t K = (N + M - 1)/M;
		subroot->size = (N+K-1)/K;
		subroot->children = new RNode[M];
		size_t i;
		for (i = 0; i < (N-K); i+=K){
			size_t j = i/K;
			subroot->children[j].parent = subroot;
			this->omt(subroot->children + j, K, level - 1, iter);
			aabbs.emplace_back(subroot->children[j].AABB);
		}
		subroot->children[i/K].parent = subroot;
		this->omt(subroot->children + (i/K), N - i, level - 1, iter); 
		aabbs.emplace_back(subroot->children[i/K].AABB);
		subroot->AABB = join<Dim>(aabbs.begin(), aabbs.end());
	}
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::print(){
	printNode(&root);
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::printNode(RNode* node){
	std::cout << "AABB: " << node->AABB.left << " " << node->AABB.top << " " << node->AABB.width << " " << node->AABB.height << std::endl;
	if (node->level == 0){
		std::cout << "Leaf: " << std::endl;
		for (auto& i : node->element_indices){
			std::cout << i << " - ";
			Rect<Dim> AABB = m_elements[i].getAABB();
			std::cout << "AABB: " << AABB.left << " " << AABB.top << " " << AABB.width << " " << AABB.height << std::endl;
		}
		std::cout << std::endl;
	} else {
		std::cout << "Branch: " << node->level << " {" << std::endl;
		for (size_t i = 0; i < node->size; i++){
			printNode(node->children + i);
		}
		std::cout << "}" << std::endl;
	}
}

template<class T, size_t M, typename Dim>
std::vector<std::reference_wrapper<T>> RTree<T,M,Dim>::intersect(const Rect<Dim>& aabb){
	std::vector<RNode*> to_search;
	std::vector<std::reference_wrapper<T>> leaf_nodes;
	to_search.emplace_back(&root);
//	int visited = 1;
	while (!to_search.empty()){
		RNode* node = to_search.back();
		to_search.pop_back();
		if (node->level == 0){
			for (auto& i : node->element_indices){
//				visited++;
				if (aabb.Intersects(m_elements[i].getAABB())){
					leaf_nodes.emplace_back(m_elements[i]);
				}
			}
		} else {
			for (size_t i = 0; i < node->size; i++){
//				visited++;
				RNode* cnode = node->children + i;
				if (aabb.Intersects(cnode->AABB)){
					to_search.emplace_back(cnode);
				}
			}
		}
	}
//	std::cout << visited << std::endl;
	return leaf_nodes;
}

template<class T, size_t M, typename Dim>
RTree<T,M,Dim>::~RTree(){
	clear_rnode(&root);
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::clear_rnode(RNode* node){
	if (node->level != 0){
		for (size_t i = 0; i < node->size; i++){
			this->clear_rnode(node->children + i);
		}
		delete[] node->children;
	}
}
#endif
