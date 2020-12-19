#ifndef UTIL_CLIB_HPP
#define UTIL_CLIB_HPP
#include <cstdint>
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
uint32_t nextPow2(uint32_t v);
#endif
