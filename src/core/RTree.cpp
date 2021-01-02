#include <core/RTree.hpp>
#include <util/RectCompare.hpp>
#include <numeric>
#include <cmath>
#include <ostream>

template<typename T>
std::vector<int> RTree<T>::load(const std::vector<Rect<T>>& elements) {
	m_nodes = {};
	m_elements = {};
	std::vector<int> indices;
	
	for (size_t idx = 0; idx < elements.size(); idx++) {
		if (this->m_leaves.count(elements[idx]) == 1) {
			indices.emplace_back(this->m_leaves[elements[idx]]);
		} else {
			size_t newid = alloc_leaf();
			indices.emplace_back(idx);
			m_elements[newid] = RLeaf<T>({static_cast<int>(idx), elements[idx]});
			this->m_leaves[elements[idx]] = idx;
		}
	}
	this->m_lastid = elements.size();

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
void RTree<T>::omt(int subroot, size_t N, size_t level, std::vector<size_t>::iterator& iter) {
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
void RTree<T>::print(std::ostream& os) const {
	os << m_nodes.size() << std::endl;
	printNode(0, os);
}

template<typename T>
void RTree<T>::printNode(size_t _node, std::ostream& os) const {
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
std::vector<int> RTree<T>::intersect(const Rect<T>& aabb) const {
	std::vector<size_t> to_search;
	std::vector<int> leaf_nodes = {};
	to_search.emplace_back(0);
	while (!to_search.empty()){
		size_t parentid = to_search.back();
		auto& node = m_nodes[parentid];
		to_search.pop_back();
		if (node.level == 0){
			for (auto& idx : node.children){
				if (aabb.Intersects(m_elements.at(idx).AABB)){
					leaf_nodes.emplace_back(m_elements.at(idx).id);
				}
			}
		} else {
			const std::vector<size_t>& children = this->intersect(parentid, aabb);
			to_search.reserve(to_search.size() + children.size());
			to_search.insert(to_search.end(), children.begin(), children.end());
		}
	}
	return leaf_nodes;
}


template<typename T>
bool RTree<T>::contains(const Rect<T>& object) const {
	return (this->m_leaves.count(object) == 1);
}

template<typename T>
T RTree<T>::overlapCost(size_t idx, const Rect<T>& object) const {
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
T RTree<T>::areaCost(size_t idx, const Rect<T>& object) const {
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
std::vector<size_t> RTree<T>::chooseSubTree(size_t idx, bool leaf) const {
	size_t curr = 0; // Start at the root
	size_t level = this->getLevel(idx, leaf) + 1;
	const Rect<T>& object = this->getAABB(idx, leaf);
	std::vector<size_t> path = {};

	while (m_nodes[curr].level > level) { // End at idx's parent
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
	if (!(this->contains(object))) {
		size_t idx = alloc_leaf();
		int nodeID = this->m_lastid++;
		this->m_elements[idx] = RLeaf<T>({nodeID, object});
		this->m_leaves[object] = nodeID;
		if (this->m_nodes[0].size() == 0) { // First node
			this->m_nodes[0].children.emplace_back(idx);
			this->m_nodes[0].AABB = object;
		} else {
			this->insertNode(idx, true);
		}
		return nodeID;
	} else {
		return this->m_leaves[object];
	}
}

template<typename T>
int RTree<T>::erase(const Rect<T>& object) {
	// Invoke FindLeaf
	const auto path = this->findPath(object);
	size_t m = static_cast<size_t>(this->M * RT_SMALL_M);
	std::vector<size_t> removedNodes;
	std::vector<size_t> removedLeaves;

	// If no leaf was found, return 0
	if (path.empty()) {
		return 0;
	}

	// Remove E from L
	this->m_leaves.erase(object);
	this->free_leaf(m_nodes[path[1]].children[path[0]]);
	m_nodes[path[1]].children.erase(m_nodes[path[1]].children.begin() + path[0]);
	
	// Condense tree
	for (auto iter = path.begin() + 1; iter != path.end(); iter++) {
		size_t idx = *iter;
		if ((idx != RT_ROOT_NODE) && (m_nodes[idx].size() < m)) {
			size_t parent = *(iter + 1);
			size_t cidx = std::distance(m_nodes[parent].children.begin(),
			std::find(m_nodes[parent].children.begin(), m_nodes[parent].children.end(), idx));
			m_nodes[parent].children.erase(m_nodes[parent].children.begin() + cidx);

			for (auto& id : m_nodes[idx].children) {
				if (m_nodes[idx].level == 0) {
					removedLeaves.emplace_back(id);
				} else {
					removedNodes.emplace_back(id);
				}
			}
		} else {
			m_nodes[idx].AABB = this->makeBound(m_nodes[idx].children.begin(),
			m_nodes[idx].children.end(),m_nodes[idx].level == 0);
		}
	}
	
	// Reinsert orphaned nodes
	for (auto& idx : removedNodes) {
		this->insertNode(idx, false);
	}
	for (auto& idx : removedLeaves) {
		this->insertNode(idx, true);
	}
	
	// If root has only one child, make that child the root
	if (this->m_nodes[0].size() == 1) {
		std::swap(this->m_nodes[0], this->m_nodes[this->m_nodes[0].children[0]]);
		this->height--;
	}
		
	return 1;
}

template<typename T>
void RTree<T>::insertNode(size_t node, bool leaf, bool first) {
	auto path = this->chooseSubTree(node, leaf);
	auto AABB = this->getAABB(node, leaf);

	this->m_nodes[path.back()].children.emplace_back(node);
	for (auto i = path.rbegin(); i != path.rend(); i++) {
		size_t nodeN = *i;
		this->m_nodes[nodeN].AABB = join(m_nodes[nodeN].AABB, AABB);
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
					this->height++;
					std::swap(m_nodes[newrt], m_nodes[nodeN]);
				} else { // Add the new node to the parent's children
					size_t parent = *(i+1);
					this->m_nodes[parent].children.emplace_back(snode);
				}
			}
		}
	}
}

template<typename T>
void RTree<T>::reinsert(size_t nodeIdx) {
	std::vector<size_t> removed_items;
	auto& node = this->m_nodes[nodeIdx];
	size_t p = std::max(static_cast<int>(node.size() * RT_REINSERT_P), 1);
	bool leaf = (node.level == 0);
	auto nodeX = node.AABB.left + node.AABB.right;
	auto nodeY = node.AABB.top + node.AABB.bottom;

	// Sort the entries in order of their distances from the center of the bounding rectangle of N
	std::partial_sort(node.children.rbegin(), node.children.rbegin() + p, node.children.rend(),
		[&](const size_t A, const size_t B) {
			const Rect<T>& nA = this->getAABB(A, leaf);
			const Rect<T>& nB = this->getAABB(B, leaf);
			double dx1 = (nA.left + nA.right) - nodeX;
			double dy1 = (nA.top + nA.bottom) - nodeY;
			double dx2 = (nB.left + nB.right) - nodeX;
			double dy2 = (nB.top + nB.bottom) - nodeY;
			return (dx1*dx1 + dy1*dy1) < (dx2*dx2 + dy2*dy2);
	});
	
	// Remove the last p entries from N
	removed_items.assign(node.children.end() - p, node.children.end());
	node.children.erase(node.children.end() - p, node.children.end());
	
	// Recompute the bounding box of N
	node.AABB = this->makeBound(node.children.begin(), node.children.end(), leaf);
	
	// Close ReInsert: Starting with the minimum distance, invoke Insert to reinsert the entries
	for (auto& idx : removed_items) {
		this->insertNode(idx, leaf, false);
	}
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
				return rectCompare(getAABB(A, hasLeaves), getAABB(B, hasLeaves), axis, dist);
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
			return rectCompare(getAABB(A, hasLeaves), getAABB(B, hasLeaves), min_axis, min_dist);
		});
	}
	
	// Split the node
	newIdx = this->m_nodes.size();
	this->m_nodes.emplace_back();
	this->m_nodes[newIdx].level = this->m_nodes[nodeIdx].level;
	this->m_nodes[newIdx].children.assign(m_nodes[nodeIdx].children.begin() + min_split + 1, m_nodes[nodeIdx].children.end());
	m_nodes[nodeIdx].children.erase(m_nodes[nodeIdx].children.begin() + min_split + 1, m_nodes[nodeIdx].children.end());

	// Recreate the bounding boxes
	this->m_nodes[newIdx].AABB = this->makeBound(this->m_nodes[newIdx].children.begin(), this->m_nodes[newIdx].children.end(), hasLeaves);
	m_nodes[nodeIdx].AABB = this->makeBound(m_nodes[nodeIdx].children.begin(), m_nodes[nodeIdx].children.end(), hasLeaves);

	return newIdx;
}

template<typename T>
Rect<T> RTree<T>::makeBound(std::vector<size_t>::const_iterator start, std::vector<size_t>::const_iterator end, bool leaves) const {
	Rect<T> bounds = {	
		std::numeric_limits<T>::max(), std::numeric_limits<T>::max(),
		std::numeric_limits<T>::min(), std::numeric_limits<T>::min()
	};

	if (start == end) {
		return bounds;
	}

	bounds = this->getAABB(*start, leaves);
	for (auto i = start + 1; i != end; i++) {
		bounds = join(bounds, this->getAABB(*i, leaves));
	}
	
	return bounds;
}

template<typename T>
int RTree<T>::getLevel(size_t idx, bool leaf) const {
	if (leaf) {
		return -1;
	} else {
		return this->m_nodes[idx].level;
	}
}

template<typename T>
Rect<T> RTree<T>::getAABB(size_t idx, bool leaf) const {
	if (leaf) {
		return this->m_elements[idx].AABB;
	} else {
		return this->m_nodes[idx].AABB;
	}
}

template<typename T>
std::vector<size_t> RTree<T>::intersect(size_t idx, const Rect<T>& object) const {
	std::vector<size_t> cnodes = {};
	for (auto& cIdx : this->m_nodes[idx].children) {
		if (object.Intersects(this->m_nodes[cIdx].AABB)) {
			cnodes.emplace_back(cIdx);
		}
	}
	return cnodes;
}

template<typename T>
std::vector<size_t> RTree<T>::findPath(const Rect<T>& object) const {
	// Map of child -> parent ids
	std::vector<std::unordered_map<size_t, size_t>> paths = {};
	std::vector<size_t> current = {RT_ROOT_NODE};
	std::vector<size_t> next = {};
	
	if (this->m_nodes.empty()) {
		return next;
	}

	// Find path from root to level 0 nodes
	for (int level = this->m_nodes[RT_ROOT_NODE].level; level > 0; level--) {
		paths.emplace_back();
		next.clear();
		for (const auto& idx : current) {
			const std::vector<size_t>& tmp = this->intersect(idx, object);
			for (const auto& id : tmp) {
				paths.back()[id] = idx;
				next.emplace_back(id);
			}
		}
		current = next;
		// If the path does not continue, return nothing
		if (current.empty()) {
			return current;
		}
	}
	
	// Find leaf which is equal to object
	next.clear();
	for (const auto& node : current) {
		for (size_t i = 0; i < this->m_nodes[node].size(); i++) {
			size_t idx = this->m_nodes[node].children[i];			
			if (this->m_elements[idx].AABB == object) {
				next.emplace_back(i);
				next.emplace_back(node);
				break;
			}
		}
	}

	// If no leaf was found, return nothing
	if (next.empty()) {
		return next;
	}
	
	// Traverse back up to the root
	for (auto pathit = paths.rbegin(); pathit != paths.rend(); pathit++) {
		next.emplace_back((*pathit)[next.back()]);
	}
	
	return next;
}

template<typename T>
size_t RTree<T>::alloc_leaf(void) {
	size_t idx = this->m_free.size();
	if (this->m_num_free > 0) {
		auto it = std::find(this->m_free.begin(), this->m_free.end(), true);
		*it = false;
		this->m_num_free--;
		return std::distance(this->m_free.begin(), it);
	} else {
		this->m_free.emplace_back(false);
		this->m_elements.emplace_back();
		return idx;
	}
}

template<typename T>
void RTree<T>::free_leaf(size_t idx) {
	if (idx == (this->m_free.size() - 1)) {
		this->m_free.erase(this->m_free.begin() + idx);
		this->m_elements.erase(this->m_elements.begin() + idx);
	} else if (idx < this->m_free.size()) {
		this->m_num_free++;
		this->m_free[idx] = true;
	}
}


template class RTree<float>;
template class RTree<int>;
template class RTree<uint64_t>;
