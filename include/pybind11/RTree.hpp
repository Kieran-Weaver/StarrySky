#ifndef PYBIND11_RTREE_HPP
#define PYBIND11_RTREE_HPP
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <core/RTree.hpp>
namespace py = pybind11;

template<typename T>
void pydef_RTree(py::module& m, const std::string& typestr) {
	using Class = RTree<T>;
	py::class_<Class>(m, typestr.c_str())
		.def(py::init<size_t>(), py::arg("_M") = 20)
		.def("intersect", py::overload_cast<const Rect<T>&>(&Class::intersect, py::const_))
		.def("load",                     &Class::load)
		.def("insert",                   &Class::insert)
		.def("contains",                 &Class::contains)
		.def("erase",                    &Class::erase)
		.def_property_readonly("bounds", &Class::getbounds)
		.def_readonly("height",          &Class::height);
}
#endif
