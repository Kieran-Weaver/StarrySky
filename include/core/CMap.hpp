#ifndef STARRYSKY_CMAP_HPP
#define STARRYSKY_CMAP_HPP
#include <core/RTree.hpp>
#include <vector>
#include <functional>
#include <unordered_map>
#include <visit_struct/visit_struct.hpp>
enum WallType { LWALL=1, RWALL=2, CEIL=4, FLOOR=8, ONEWAY=16  };

struct Surface{
	Rect<float> hitbox;
	int flags;
};
VISITABLE_STRUCT(Surface, hitbox, flags);

class CMap {
public:
	void load(const std::vector<Surface>& surfaces);
	int  insert(const Surface& wall);
	std::vector<std::reference_wrapper<const Surface>> collide(const Rect<float>& rect) const;	
	const std::unordered_map<int, Surface>& getSurfaces(void) const;
	Rect<float> getbounds(void) const;
private:
	FloatRTree internal;
	std::unordered_map<int, Surface> surfmap;
};

#endif
