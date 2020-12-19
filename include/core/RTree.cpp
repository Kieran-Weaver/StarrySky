#ifndef RTREE_IMPL
#define RTREE_IMPL
#include <core/RTree.hpp>
#include <util/RectCompare.hpp>
#include <numeric>
#include <cmath>
#include <iostream>
template<class T, typename Dim>
std::vector<int> RTree<T,Dim>::load(const std::vector<Rect<Dim>>& elements){
	m_nodes = {};
	m_elements = {};
	std::vector<int> indices;
	for (size_t idx = 0; idx < elements.size(); idx++) {
		indices.emplace_back(idx);
		RLeaf<Dim> tmp;
		tmp.id = idx;
		tmp.AABB = elements[idx];
		m_elements.emplace_back(tmp);
	}

	size_t N = m_elements.size();
	size_t H = std::ceil(std::log2(N) / std::log2(M));
	double N_subtree = std::pow(M,H-1);
	size_t S = std::floor(std::sqrt(std::ceil(N/N_subtree)));
	this->height = H;
	this->m_nodes.emplace_back(S);
	this->m_nodes[0].level = H;
	std::sort(m_elements.begin(), m_elements.end(), [&](const RLeaf<Dim>& lhs, const RLeaf<Dim>& rhs){
		return rectCompare<Dim>(lhs.AABB, rhs.AABB, H & 0x01);
	});
	std::vector<size_t> tempVec;
	tempVec.resize(N);
	std::iota(tempVec.begin(), tempVec.end(), 0);
	auto iter = tempVec.begin();
	size_t i;
	std::vector<Rect<Dim>> aabbs;
	std::iota(this->m_nodes[0].children.begin(), this->m_nodes[0].children.end(), this->m_nodes.size());
	this->m_nodes.resize(this->m_nodes.size() + S, 0);
	for (i = 0; i < S; i++){
		int nodeIdx = m_nodes[0].children[i];
		this->m_nodes[nodeIdx].parent = 0;
		this->omt(nodeIdx, std::min(N/S, static_cast<size_t>(tempVec.end() - iter)), H-1, iter);
		aabbs.emplace_back(this->m_nodes[nodeIdx].AABB);
	}
	this->m_nodes[0].AABB = join<Dim>(aabbs.begin(), aabbs.end());
	
	return indices;
}

template<class T, typename Dim>
void RTree<T,Dim>::omt(int subroot, size_t N, size_t level, std::vector<size_t>::iterator& iter){
	std::vector<Rect<Dim>> aabbs;
	if (N < M){
		RNode<Dim> leaf(N);
		aabbs.reserve(N);
		for (size_t i = 0; i < N; i++){
			leaf.children[i] = (*iter);
			iter++;
			aabbs.emplace_back(m_elements[leaf.children[i]].AABB);
		}
		leaf.AABB = join<Dim>(aabbs.begin(), aabbs.end());
		int parent = subroot;
		while (level != 0){
			m_nodes[subroot].children.resize(1);
			m_nodes[subroot].level = level;
			m_nodes[subroot].AABB = leaf.AABB;
			m_nodes[subroot].children[0] = m_nodes.size();
			m_nodes.emplace_back(0);
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
			return rectCompare<Dim>(m_elements[lhs].AABB,m_elements[rhs].AABB, level & 0x01);
		});
		size_t K = (N + M - 1)/M;
		m_nodes[subroot].children.resize((N+K-1)/K);
		size_t i;
		std::iota(this->m_nodes[subroot].children.begin(), this->m_nodes[subroot].children.end(), this->m_nodes.size());
		this->m_nodes.resize(this->m_nodes.size() + m_nodes[subroot].size(), M);
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

template<class T, typename Dim>
void RTree<T,Dim>::print(){
	std::cout << m_nodes.size() << std::endl;
	printNode(0);
}

template<class T, typename Dim>
void RTree<T,Dim>::printNode(int _node){
	auto& node = m_nodes[_node];
	std::cout << "AABB: " << node.AABB.left << " " << node.AABB.top << " " << node.AABB.right << " " << node.AABB.bottom << std::endl;
	if (node.level == 0){
		std::cout << "Leaf: " << std::endl;
		for (size_t j = 0; j < node.size(); j++){
			auto& i = node.children[j];
			std::cout << i << " - ";
			auto AABB = m_elements[i].AABB;
			std::cout << "AABB: " << AABB.left << " " << AABB.top << " " << AABB.right << " " << AABB.bottom << std::endl;
		}
		std::cout << std::endl;
	} else {
		std::cout << "Branch: " << node.level << " {" << std::endl;
		for (size_t i = 0; i < node.size(); i++){
			printNode(node.children[i]);
		}
		std::cout << "}" << std::endl;
	}
}

template<class T, typename Dim>
std::vector<int> RTree<T,Dim>::intersect(const Rect<Dim>& aabb){
	std::vector<int> to_search;
	std::vector<int> leaf_nodes = {};
	to_search.emplace_back(0);
	while (!to_search.empty()){
		auto& node = m_nodes[to_search.back()];
		to_search.pop_back();
		if (node.level == 0){
			for (size_t j = 0; j < node.size(); j++){
				auto& i = node.children[j];
				if (aabb.Intersects(m_elements.at(i).AABB)){
					leaf_nodes.emplace_back(m_elements.at(i).id);
				}
			}
		} else {
			for (size_t i = 0; i < node.size(); i++){
				auto& cnode = m_nodes[node.children[i]];
				if (aabb.Intersects(cnode.AABB)){
					to_search.emplace_back(node.children[i]);
				}
			}
		}
	}
	return leaf_nodes;
}
#endif
