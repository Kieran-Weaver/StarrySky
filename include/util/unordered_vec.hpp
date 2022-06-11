#ifndef STARRYSKY_UNORDERED_VEC_H
#define STARRYSKY_UNORDERED_VEC_H

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <util/ID.hpp>

template<typename T>
class unordered_vec {
public:
	const T& operator[](id_t id) const {
		return m_vec[ m_map.at(id) ].second;
	}
	T& operator[](id_t id) {
		return m_vec[ m_map[id] ].second;
	}
	template<typename U>
	id_t insert(U&& item) {
		id_t id = new_id();
		m_map[id] = m_vec.size();
		m_vec.emplace_back(id, std::forward(item));
		return id;
	}
	size_t count(id_t id) {
		return m_map.count(id);
	}
	size_t size(void) {
		return m_vec.size();
	}
	void erase(id_t id) {
		size_t pos = m_map[id];
		std::swap( m_vec[pos], m_vec[m_vec.size()-1] );
		id_t previd = m_vec[pos].first;
		m_map[previd] = pos;
	}
private:
	std::vector<std::pair<id_t, T>> m_vec;
	std::unordered_map<id_t, size_t> m_map;
};

#endif
