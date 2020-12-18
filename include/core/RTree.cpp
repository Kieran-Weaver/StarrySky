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
	this->m_nodes.emplace_back();
	this->m_nodes[0].level = H;
	std::sort(m_elements.begin(), m_elements.end(), [&](const T& lhs, const T& rhs){
		return rectCompare<Dim>(lhs.getAABB(),rhs.getAABB(), H & 0x01);
	});
	std::vector<size_t> tempVec;
	tempVec.resize(N);
	std::iota(tempVec.begin(), tempVec.end(), 0);
	auto iter = tempVec.begin();
	size_t i;
	std::vector<Rect<Dim>> aabbs;
	std::iota(this->m_nodes[0].children.begin(), this->m_nodes[0].children.end(), this->m_nodes.size());
	this->m_nodes.resize(this->m_nodes.size() + S);
	for (i = 0; i < S; i++){
		int nodeIdx = m_nodes[0].children[i];
		this->m_nodes[nodeIdx].parent = 0;
		this->omt(nodeIdx, std::min(N/S, static_cast<size_t>(tempVec.end() - iter)), H-1, iter);
		aabbs.emplace_back(this->m_nodes[nodeIdx].AABB);
	}
	this->m_nodes[0].size = S;
	this->m_nodes[0].AABB = join<Dim>(aabbs.begin(), aabbs.end());
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::omt(int subroot, size_t N, size_t level, std::vector<size_t>::iterator& iter){
	std::vector<Rect<Dim>> aabbs;
	if (N < M){
		RNode leaf = { -1, 0, N, {}, {} };
		aabbs.reserve(N);
		for (size_t i = 0; i < N; i++){
			leaf.children[i] = (*iter);
			iter++;
			aabbs.emplace_back(m_elements[leaf.children[i]].getAABB());
		}
		leaf.AABB = join<Dim>(aabbs.begin(), aabbs.end());
		int parent = subroot;
		while (level != 0){
			m_nodes[subroot].size = 1;
			m_nodes[subroot].level = level;
			m_nodes[subroot].AABB = leaf.AABB;
			m_nodes[subroot].children[0] = m_nodes.size();
			m_nodes.emplace_back();
			m_nodes.back().parent = subroot;
			level--;
			parent = subroot;
			subroot = m_nodes[subroot].children[0];
		}
		m_nodes[subroot] = leaf;
		m_nodes[subroot].parent = parent;
	} else {
		m_nodes[subroot].level = level;
		std::sort(iter, iter + N, [&](const size_t& lhs, const size_t& rhs){
			return rectCompare<Dim>(m_elements[lhs].getAABB(),m_elements[rhs].getAABB(), level & 0x01);
		});
		size_t K = (N + M - 1)/M;
		m_nodes[subroot].size = (N+K-1)/K;
		size_t i;
		std::iota(this->m_nodes[subroot].children.begin(), this->m_nodes[subroot].children.end(), this->m_nodes.size());
		this->m_nodes.resize(this->m_nodes.size() + m_nodes[subroot].size);
		for (i = 0; i < N; i+=K){
			size_t j = i/K;
			int nodeIdx = m_nodes[subroot].children[j];
			m_nodes[nodeIdx].parent = subroot;
			this->omt(nodeIdx, std::min(K, N-i), level - 1, iter);
			aabbs.emplace_back(m_nodes[nodeIdx].AABB);
		}
		m_nodes[subroot].AABB = join<Dim>(aabbs.begin(), aabbs.end());
	}
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::print(){
	std::cout << m_nodes.size() << std::endl;
	printNode(0);
}

template<class T, size_t M, typename Dim>
void RTree<T,M,Dim>::printNode(int _node){
	RNode& node = m_nodes[_node];
	std::cout << "AABB: " << node.AABB.left << " " << node.AABB.top << " " << node.AABB.right << " " << node.AABB.bottom << std::endl;
	if (node.level == 0){
		std::cout << "Leaf: " << std::endl;
		for (int j = 0; j < node.size; j++){
			auto& i = node.children[j];
			std::cout << i << " - ";
			Rect<Dim> AABB = m_elements[i].getAABB();
			std::cout << "AABB: " << AABB.left << " " << AABB.top << " " << AABB.right << " " << AABB.bottom << std::endl;
		}
		std::cout << std::endl;
	} else {
		std::cout << "Branch: " << node.level << " {" << std::endl;
		for (size_t i = 0; i < node.size; i++){
			printNode(node.children[i]);
		}
		std::cout << "}" << std::endl;
	}
}

template<class T, size_t M, typename Dim>
std::vector<std::reference_wrapper<T>> RTree<T,M,Dim>::intersect(const Rect<Dim>& aabb){
	std::vector<int> to_search;
	std::vector<std::reference_wrapper<T>> leaf_nodes;
	to_search.emplace_back(0);
//	int visited = 1;
	while (!to_search.empty()){
		RNode& node = m_nodes[to_search.back()];
		to_search.pop_back();
		if (node.level == 0){
			for (size_t j = 0; j < node.size; j++){
				auto& i = node.children[j];
//				visited++;
				if (aabb.Intersects(m_elements.at(i).getAABB())){
					leaf_nodes.emplace_back(m_elements.at(i));
				}
			}
		} else {
			for (size_t i = 0; i < node.size; i++){
//				visited++;
				RNode& cnode = m_nodes[node.children[i]];
				if (aabb.Intersects(cnode.AABB)){
					to_search.emplace_back(node.children[i]);
				}
			}
		}
	}
//	std::cout << visited << std::endl;
	return leaf_nodes;
}
#endif
