#ifndef STARRYSKY_RECTCOMPARE_HPP
#define STARRYSKY_RECTCOMPARE_HPP
#include <util/Rect.hpp>
#include <tuple>

// Compare two Rect<T>s, on dimension D (false = x, true = y),
// Using the value K (false = lower bound, true = upper bound)
template<typename T, bool D, bool K>
struct rectCompareImpl{
	static bool _(const Rect<T>& lhs, const Rect<T>& rhs);
};

template<typename T, bool D, bool K>
bool rectCompareImpl<T,D,K>::_(const Rect<T>& lhs, const Rect<T>& rhs){
	if constexpr (D) {
		if constexpr (K) {
			return std::tie(lhs.bottom, lhs.right) < std::tie(rhs.bottom, rhs.right);
		} else {
			return std::tie(lhs.top, lhs.left) < std::tie(rhs.top, rhs.left);
		}
	} else {
		if constexpr (K) {
			return std::tie(lhs.right, lhs.bottom) < std::tie(rhs.right, rhs.bottom);
		} else {
			return std::tie(lhs.left, lhs.top) < std::tie(rhs.left, rhs.top);
		}
	}
}

template<typename T>
bool rectCompare(const Rect<T>& lhs, const Rect<T>& rhs, bool dim, bool K = false){
	if (dim){
		if (K) {
			return rectCompareImpl<T, true, true>::_(lhs, rhs);
		} else {
			return rectCompareImpl<T, true, false>::_(lhs, rhs);
		}
	} else {
		if (K) {
			return rectCompareImpl<T, false, true>::_(lhs, rhs);
		} else {
			return rectCompareImpl<T, false, false>::_(lhs, rhs);
		}
	}
}
#endif
