#include <core/RTree.hpp>
#include <util/RectCompare.hpp>
#include <numeric>
#include <cmath>
template<typename T>
std::vector<int> RTree<T>::load(const std::vector<Rect<T>>& elements){
	m_nodes = {};
	m_elements = {};
	std::vector<int> indices;
	for (size_t idx = 0; idx < elements.size(); idx++) {
		indices.emplace_back(idx);
		RLeaf<T> tmp;
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
	std::sort(m_elements.begin(), m_elements.end(), [&](const RLeaf<T>& lhs, const RLeaf<T>& rhs){
		return rectCompare<T>(lhs.AABB, rhs.AABB, H & 0x01);
	});
	std::vector<size_t> tempVec;
	tempVec.resize(N);
	std::iota(tempVec.begin(), tempVec.end(), 0);
	auto iter = tempVec.begin();
	size_t i;
	std::vector<Rect<T>> aabbs;
	std::iota(this->m_nodes[0].children.begin(), this->m_nodes[0].children.end(), this->m_nodes.size());
	this->m_nodes.resize(this->m_nodes.size() + S, 0);
	for (i = 0; i < S; i++){
		int nodeIdx = m_nodes[0].children[i];
		this->omt(nodeIdx, std::min(N/S, static_cast<size_t>(tempVec.end() - iter)), H-1, iter);
		aabbs.emplace_back(this->m_nodes[nodeIdx].AABB);
	}
	this->m_nodes[0].AABB = join<T>(aabbs.begin(), aabbs.end());
	
	return indices;
}

template<typename T>
void RTree<T>::omt(int subroot, size_t N, size_t level, std::vector<size_t>::iterator& iter){
	std::vector<Rect<T>> aabbs;
	if (N < M){
		RNode<T> leaf(N);
		aabbs.reserve(N);
		for (size_t i = 0; i < N; i++){
			leaf.children[i] = (*iter);
			iter++;
			aabbs.emplace_back(m_elements[leaf.children[i]].AABB);
		}
		leaf.AABB = join<T>(aabbs.begin(), aabbs.end());
		while (level != 0){
			m_nodes[subroot].children.resize(1);
			m_nodes[subroot].level = level;
			m_nodes[subroot].AABB = leaf.AABB;
			m_nodes[subroot].children[0] = m_nodes.size();
			m_nodes.emplace_back(0);
			level--;
			subroot = m_nodes[subroot].children[0];
		}
		m_nodes[subroot] = leaf;
	} else {
		m_nodes[subroot].level = level;
		std::sort(iter, iter + N, [&](const size_t& lhs, const size_t& rhs){
			return rectCompare<T>(m_elements[lhs].AABB,m_elements[rhs].AABB, level & 0x01);
		});
		size_t K = (N + M - 1)/M;
		m_nodes[subroot].children.resize((N+K-1)/K);
		size_t i;
		std::iota(this->m_nodes[subroot].children.begin(), this->m_nodes[subroot].children.end(), this->m_nodes.size());
		this->m_nodes.resize(this->m_nodes.size() + m_nodes[subroot].size(), M);
		for (i = 0; i < N; i+=K){
			size_t j = i/K;
			int nodeIdx = m_nodes[subroot].children[j];
			this->omt(nodeIdx, std::min(K, N-i), level - 1, iter);
			aabbs.emplace_back(m_nodes[nodeIdx].AABB);
		}
		m_nodes[subroot].AABB = join<T>(aabbs.begin(), aabbs.end());
	}
}

template<typename T>
void RTree<T>::print(std::ostream& os){
	os << m_nodes.size() << std::endl;
	printNode(0, os);
}

template<typename T>
void RTree<T>::printNode(size_t _node, std::ostream& os){
	const auto& node = m_nodes[_node];
	os << "AABB: " << node.AABB.left << " " << node.AABB.top << " " << node.AABB.right << " " << node.AABB.bottom << std::endl;
	if (node.level == 0){
		os << "Leaf: " << std::endl;
		for (size_t j = 0; j < node.size(); j++){
			const auto& i = node.children[j];
			os << i << " - ";
			const auto& AABB = m_elements[i].AABB;
			os << "AABB: " << AABB.left << " " << AABB.top << " " << AABB.right << " " << AABB.bottom << std::endl;
		}
		os << std::endl;
	} else {
		os << "Branch: " << node.level << " {" << std::endl;
		for (size_t i = 0; i < node.size(); i++){
			printNode(node.children[i], os);
		}
		os << "}" << std::endl;
	}
}

template<typename T>
std::vector<int> RTree<T>::intersect(const Rect<T>& aabb){
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

template<typename T>
T RTree<T>::overlapCost(size_t idx, const Rect<T>& object) {
	if ((idx >= m_nodes.size()) || (m_nodes[idx].level == 0)) {
		return 0;
	} else {
		T area = 0;
		Rect<T> null_rect = Rect<T>({T(0), T(0), T(0), T(0)});
		for (size_t i : m_nodes[idx].children) {
			const auto& E_f = m_nodes[i].AABB;
			area += E_f.RIntersects(object).value_or(null_rect).Area();
		}
		return area;
	}
}

template<typename T>
T RTree<T>::areaCost(size_t idx, const Rect<T>& object) {
	if ((idx >= m_nodes.size()) || (m_nodes[idx].level == 0)) {
		return 0;
	} else {
		Rect<T> obj = object;
		for (size_t i : m_nodes[idx].children) {
			obj = join(obj, m_nodes[i].AABB);
		}
		return obj.Area();
	}
}

template<typename T>
std::vector<size_t> RTree<T>::chooseSubTree(size_t id, bool leaf) {
	size_t curr = 0; // Start at the root
	size_t level = leaf ? 0 : m_nodes[id].level;
	const Rect<T>& object = m_nodes[id].AABB;
	std::vector<size_t> path = {};

	while (m_nodes[curr].level > level) { // End at id's parent
		path.emplace_back(curr);
		std::vector<T> areas = {};
		for (size_t idx : m_nodes[curr].children) {
			areas.emplace_back(this->areaCost(idx, object));
		}
		if (m_nodes[curr].level == 1) { // If the childpointers in N point to leaves
			std::vector<size_t> idxs(m_nodes[curr].children.size());
			size_t upperBound = std::min(idxs.size(), size_t{RT_OVERLAP_P});
			std::vector<T> overlaps = {};

			std::iota(idxs.begin(), idxs.end(), 0);
			std::partial_sort(idxs.begin(), idxs.begin() + upperBound, idxs.end(),
			[&](const size_t A, const size_t B) {
				return areas[A] < areas[B];
			});
			
			for (size_t i = 0; i < upperBound; i++) {
				size_t idx = m_nodes[curr].children[idxs[i]];
				overlaps.emplace_back(this->overlapCost(idx, object));
			}
			
			int index = std::min_element(overlaps.begin(), overlaps.end()) - overlaps.begin();
			curr = m_nodes[curr].children[index];
		} else { // If the childpointers in N do not point to leaves
			int index = std::min_element(areas.begin(), areas.end()) - areas.begin();
			curr = m_nodes[curr].children[index];
		}
	}
	path.emplace_back(curr);
	return path;
}

template<typename T>
int RTree<T>::insert(const Rect<T>& object) {
	int nodeID = static_cast<int>(this->m_elements.size());
	this->m_elements.emplace_back(RLeaf<T>({nodeID, object}));
	if (this->m_nodes[0].size() == 0) { // First node
		this->m_nodes[0].children.emplace_back(nodeID);
		this->m_nodes[0].AABB = object;
	} else {
		this->insertNode(nodeID);
	}
	return nodeID;
}

template<typename T>
void RTree<T>::insertNode(size_t node, bool first) {
	auto path = this->chooseSubTree(node);
	this->m_nodes[path.back()].children.emplace_back(node);
	for (auto i = path.rbegin(); i != path.rend(); i++) {
		size_t nodeN = *i;
		this->m_nodes[nodeN].AABB = join(m_nodes[nodeN].AABB, m_nodes[node].AABB);
		if (this->m_nodes[nodeN].size() > this->M) {
			if ((nodeN != RT_ROOT_NODE) && first) {
				this->reinsert(nodeN);
			} else {
				auto snode = this->split(nodeN);
				if (nodeN == 0) { // Root split, make a new root
					size_t newrt = this->m_nodes.size();
					this->m_nodes.emplace_back(2);
					this->m_nodes[newrt].AABB = join(m_nodes[nodeN].AABB, m_nodes[snode].AABB);
					this->m_nodes[newrt].children[0] = snode;
					this->m_nodes[newrt].children[1] = newrt;
					this->m_nodes[newrt].level = this->m_nodes[nodeN].level + 1;
					std::swap(m_nodes[newrt], m_nodes[nodeN]);
				}
			}
		}
	}
}

template<typename T>
void RTree<T>::reinsert(size_t node) {
	(void)node;
}

template<typename T>
size_t RTree<T>::split(size_t nodeIdx) {
	auto& node = this->m_nodes[nodeIdx];
	const std::array<bool, 2> booleans = {false, true};
	bool min_axis, min_dist;
	bool hasLeaves = (node.level == 0);
	size_t m = static_cast<size_t>(node.size() * RT_SMALL_M);
	size_t num_dists = node.size() - (2*m) + 1;
	size_t newIdx, min_split = m;
	T min_margin = std::numeric_limits<T>::max();
	for (const auto& axis : booleans) {
		// For each axis
		T margin = 0;
		bool axis_dist = false;
		size_t axis_split = m;
		T area, overlap;
		T min_overlap = std::numeric_limits<T>::max(), min_area = std::numeric_limits<T>::max();
		
		for (const auto& dist : booleans) {
			// Sort the entries by the lower then by the upper value...
			std::sort(node.children.begin(), node.children.end(), [&](const size_t A, const size_t B) {
				if (hasLeaves) {
					return rectCompare(this->m_elements[A].AABB, this->m_elements[B].AABB, axis, dist);
				} else {
					return rectCompare(this->m_nodes[A].AABB, this->m_nodes[B].AABB, axis, dist);
				}
			});
			// Compute S, the sum of all margin-values of the different distributions
			for (size_t k = 0; k < num_dists; k++) {
				size_t sp_off = m + k;
				Rect<T> R1 = this->makeBound(node.children.begin(), node.children.begin() + sp_off, hasLeaves);
				Rect<T> R2 = this->makeBound(node.children.begin() + sp_off + 1, node.children.end(), hasLeaves);
				auto R3 = R1.RIntersects(R2);

				margin += R1.Margin() + R2.Margin();
				area = R1.Area() + R2.Area();
				overlap = T(0);
				if (R3) {
					overlap = R3.value().Area();
				}
				
				if (std::tie(overlap, area) < std::tie(min_overlap, min_area)) {
					min_overlap = overlap;
					min_area = area;
					axis_dist = dist;
					axis_split = m + k;
				}
			}
		}
		
		// Choose the axis with minimum S as split axis
		if (margin < min_margin) {
			min_axis = axis;
			min_margin = margin;
			min_dist = axis_dist;
			min_split = axis_split;
		}
	}

	// If the children are not already sorted, sort them
	if ((min_dist != true) && (min_axis != true)) {
		std::sort(node.children.begin(), node.children.end(), [&](const size_t A, const size_t B) {
			if (hasLeaves) {
				return rectCompare(this->m_elements[A].AABB, this->m_elements[B].AABB, min_axis, min_dist);
			} else {
				return rectCompare(this->m_nodes[A].AABB, this->m_nodes[B].AABB, min_axis, min_dist);
			}
		});
	}
	
	// Split the node
	newIdx = this->m_nodes.size();
	this->m_nodes.emplace_back();
	this->m_nodes[newIdx].level = node.level;
	this->m_nodes[newIdx].children.assign(node.children.begin() + min_split + 1, node.children.end());
	node.children.erase(node.children.begin() + min_split + 1, node.children.end());

	// Recreate the bounding boxes
	this->m_nodes[newIdx].AABB = this->makeBound(this->m_nodes[newIdx].children.begin(), this->m_nodes[newIdx].children.end(), hasLeaves);
	node.AABB = this->makeBound(node.children.begin(), node.children.end(), hasLeaves);

	return newIdx;
}

template<typename T>
Rect<T> RTree<T>::makeBound(std::vector<size_t>::const_iterator start, std::vector<size_t>::const_iterator end, bool leaves) {
	Rect<T> bounds = {};
	if (start == end) {
		return bounds;
	}

	if (leaves) {
		bounds = this->m_elements[*start].AABB;
		for (auto i = start + 1; i != end; i++) {
				bounds = join(bounds, this->m_elements[*i].AABB);
		}
	} else {
		bounds = this->m_nodes[*start].AABB;
		for (auto i = start + 1; i != end; i++) {
				bounds = join(bounds, this->m_nodes[*i].AABB);
		}
	}
	
	return bounds;
}

template class RTree<float>;
template class RTree<int>;
template class RTree<uint64_t>;
