#ifndef RECT_H
#define RECT_H
#include <glm/glm.hpp>
#include <algorithm>
template<typename T>
struct Rect{
	Rect() : left(0), top(0), width(0), height(0){
		static_assert(std::is_arithmetic<T>::value, "Integral required.");
	}
	Rect(const Rect<T>& r) : left(r.left), top(r.top), width(r.width), height(r.height) {
		static_assert(std::is_arithmetic<T>::value, "Integral required.");
	}
	Rect(T l, T t, T w, T h) : left(l), top(t), width(w), height(h) {
		static_assert(std::is_arithmetic<T>::value, "Integral required.");
	}
	T left;
	T top;
	T width;
	T height;
	bool Contains(glm::vec2 point) const{
		return (((left - point.x)*(left + width - point.x) <= 0.f)&&((top - point.y)*(top + height - point.y) <= 0.f));
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
	bool RIntersects(const Rect<T>& r, Rect<T>& intersection) const{
		T iLeft = std::max(left, r.left);
		T iTop = std::max(top, r.top);
		T iRight = std::min(left + width,r.left + r.width);
		T iBottom = std::min(top + height, r.top + r.height);
		if ((iLeft < iRight) && (iTop < iBottom)){
			intersection = Rect<T>(iLeft,iTop,iRight - iLeft, iBottom-iTop);
			return true;
		}else{
			return false;
		}
	}
};
#endif
