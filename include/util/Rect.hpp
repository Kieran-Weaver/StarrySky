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
	VISITABLE(T, right);
	VISITABLE(T, bottom);
	END_VISITABLES;

	bool operator==(const Rect<T>& other) const {
		return (other.left == left) && (other.right == right)
			&& (other.top == top) && (other.bottom == bottom);
	}

	bool Contains(float x, float y) const{
		return (((left - x)*(right - x) <= 0.f)&&((top - y)*(bottom - y) <= 0.f));
	}
	bool Contains(const Rect<T>& r) const{
		return ((r.right <= right) && (r.bottom <= bottom) && (r.left >= left) && (r.top >= top));
	}

	bool Intersects(const Rect<T>& r) const{
		T iLeft = std::max(left, r.left);
		T iTop = std::max(top, r.top);
		T iRight = std::min(right, r.right);
		T iBottom = std::min(bottom, r.bottom);
		return ((iLeft <= iRight) && (iTop <= iBottom));
	}

	std::optional<Rect<T>> RIntersects(const Rect<T>& r) const{
		T iLeft = std::max(left, r.left);
		T iTop = std::max(top, r.top);
		T iRight = std::min(right, r.right);
		T iBottom = std::min(bottom, r.bottom);
		if ((iLeft <= iRight) && (iTop <= iBottom)){
			return Rect<T>({iLeft, iTop, iRight, iBottom});
		}else{
			return {};
		}
	}

	void Translate(T x, T y) {
		this->left += x;
		this->right += x;
		this->top += y;
		this->bottom += y;
	}

	T Area() const {
		return (right - left) * (bottom - top);
	}
	
	T Margin() const {
		return (right - left) + (bottom - top);
	}
};

inline Rect<float> Normalize(const Rect<uint16_t>& texrect){
	return {texrect.left/65536.f,texrect.top/65536.f,texrect.right/65536.f,texrect.bottom/65536.f};
}

template<class T, class Iter>
Rect<T> join(Iter i, Iter last){
	T minX = std::numeric_limits<T>::max(), maxX = std::numeric_limits<T>::min();
	T minY = std::numeric_limits<T>::max(), maxY = std::numeric_limits<T>::min();
	while (i != last){
		minX = std::min(i->left, minX);
		maxX = std::max(i->right, maxX);
		minY = std::min(i->top, minY);
		maxY = std::max(i->bottom, maxY);
		i++;
	}
	return {minX, minY, maxX, maxY};
}

template<class T>
Rect<T> join(const Rect<T>& a, const Rect<T>& b){
	T minX = std::min(a.left, b.left);
	T maxX = std::max(a.right, b.right);
	T minY = std::min(a.top, b.top);
	T maxY = std::max(a.bottom, b.bottom);
	return {minX, minY, maxX, maxY};
}

namespace std
{
	template<typename T> struct hash<Rect<T>>
	{
		std::size_t operator()(const Rect<T>& s) const noexcept
		{
			std::size_t h1 = std::hash<T>{}(s.left);
			std::size_t h2 = std::hash<T>{}(s.right);
			std::size_t h3 = std::hash<T>{}(s.top);
			std::size_t h4 = std::hash<T>{}(s.bottom);
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
		}
	};
}
#endif
