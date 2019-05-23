#ifndef RECT_H
#define RECT_H
#include <algorithm>
template<typename T>
struct Rect{
	Rect(const Rect<T>& r) : left(r.left), top(r.top), width(r.width), height(r.height) {}
	Rect(T l, T t, T w, T h) : left(l), top(t), width(w), height(h) {}
	T left;
	T top;
	T width;
	T height;
	bool Contains(glm::vec2 point);
	bool Contains(Rect<T> r);
	bool Intersects(Rect<T> r);
	Rect<T> RIntersects(Rect<T> r);
};
template<typename T>
bool Rect<T>::Contains(glm::vec2 point){
	return (((left - point.x)*(left + width - point.x) <= 0.f)&&((top - point.y)*(top + height - point.y) <= 0.f));
}
template<typename T>
bool Rect<T>::Contains(Rect<T> r){
	return (((r.left + r.width)<=(left+width))&&((r.top+r.height)<=(top+height))&&(r.left >= left) && (r.top >= top));
}
template<typename T>
bool Rect<T>::Intersects(Rect<T> r){
	T iLeft = std::max(left, r.left);
	T iTop = std::max(top, r.top);
	T iRight = std::min(left + width,r.left + r.width);
	T iBottom = std::min(top + height, r.top + r.height);
	return ((iLeft < iRight) && (iTop < iBottom));
}
template<typename T>
Rect<T> Rect<T>::RIntersects (Rect<T> r){
	T iLeft = std::max(left, r.left);
	T iTop = std::max(top, r.top);
	T iRight = std::min(left + width,r.left + r.width);
	T iBottom = std::min(top + height, r.top + r.height);
	if ((iLeft < iRight) && (iTop < iBottom)){
		return Rect<T>(iLeft,iTop,iRight - iLeft, iBottom-iTop);
	}else{
		return Rect<T>(0,0,0,0);
	}
}
#endif
