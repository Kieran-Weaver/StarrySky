#include <core/CMap.hpp>

void CMap::load(const std::vector<Surface>& surfaces) {
	std::vector<Rect<float>> hitboxes = {};
	std::vector<int> ids = {};

	for (auto& surf : surfaces) {
		hitboxes.emplace_back(surf.hitbox);
	}
	ids = this->internal.load(hitboxes);

	for (size_t i = 0; i < ids.size(); i++) {
		this->surfmap[ids[i]] = surfaces[i];
	}
}

int CMap::insert(const Surface& wall) {
	int id = this->internal.insert(wall.hitbox);
	this->surfmap[id] = wall;
	return id;
}

std::vector<std::reference_wrapper<const Surface>> CMap::collide(const Rect<float>& rect) const {
	std::vector<int> ids = this->internal.intersect(rect);
	std::vector<std::reference_wrapper<const Surface>> sr;
	for (auto& id : ids) {
		sr.emplace_back(std::cref(surfmap.at(id)));
	}
	return sr;
}

const std::unordered_map<int, Surface>& CMap::getSurfaces(void) const {
	return this->surfmap;
}

Rect<float> CMap::getbounds(void) const {
	return this->internal.getbounds();
}
