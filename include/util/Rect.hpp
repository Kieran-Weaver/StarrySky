#ifndef RECT_H
#define RECT_H
#include <algorithm>
#include <cstdint>
#include <vector>
#include <optional>
#include <visit_struct/visit_struct_intrusive.hpp>
template<typename T>
struct Rect{
	static_assert(std::is_arithmetic<T>::value, "Integral required.");
	
	BEGIN_VISITABLES(Rect<T>);
	VISITABLE(T, left);
	VISITABLE(T, top);
	VISITABLE(T, width);
	VISITABLE(T, height);
	END_VISITABLES;

	bool Contains(float x, float y) const{
		return (((left - x)*(left + width - x) <= 0.f)&&((top - y)*(top + height - y) <= 0.f));
	}
	bool Contains(const Rect<T>& r) const{
		return (((r.left + r.width)<=(left+width))&&((r.top+r.height)<=(top+height))&&(r.left >= left) && (r.top >= top));
	}

	bool Intersects(const Rect<T>& r) const{
		T iLeft = std::max(left, r.left);
		T iTop = std::max(top, r.top);
		T iRight = std::min(left + width,r.left + r.width);
		T iBottom = std::min(top + height, r.top + r.height);
		return ((iLeft < iRight) && (iTop < iBottom));
	}

	std::optional<Rect<T>> RIntersects(const Rect<T>& r) const{
		T iLeft = std::max(left, r.left);
		T iTop = std::max(top, r.top);
		T iRight = std::min(left + width,r.left + r.width);
		T iBottom = std::min(top + height, r.top + r.height);
		if ((iLeft < iRight) && (iTop < iBottom)){
			return Rect<T>({iLeft,iTop,iRight - iLeft, iBottom-iTop});
		}else{
			return {};
		}
	}

	Rect<T> getAABB() const{
		return *this;
	}
};

inline Rect<float> Normalize(const Rect<uint16_t>& texrect){
	return {texrect.left/65536.f,texrect.top/65536.f,texrect.width/65536.f,texrect.height/65536.f};
}

template<class T, class Iter>
Rect<T> join(Iter i, Iter last){
	T minX = std::numeric_limits<T>::max(), maxX = std::numeric_limits<T>::min();
	T minY = std::numeric_limits<T>::max(), maxY = std::numeric_limits<T>::min();
	while (i != last){
		minX = std::min(i->left, minX);
		maxX = std::max(i->left + i->width, maxX);
		minY = std::min(i->top, minY);
		maxY = std::max(i->top + i->height, maxY);
		i++;
	}
	return {minX, minY, maxX - minX, maxY - minY};
}

template<class T>
Rect<T> join(const Rect<T>& a, const Rect<T>& b){
	T minX = std::min(a.left, b.left);
	T maxX = std::max(a.left + a.width, b.left + b.width);
	T minY = std::min(a.top, b.top);
	T maxY = std::max(a.top + a.height, b.top + b.height);
	return {minX, minY, maxX - minX, maxY - minY};
}
#endif
