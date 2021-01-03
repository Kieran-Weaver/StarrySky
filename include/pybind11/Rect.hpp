#ifndef PYBIND11_RECT_HPP
#define PYBIND11_RECT_HPP
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <util/Rect.hpp>
namespace py = pybind11;

template<typename T>
void pydef_Rect(py::module& m, const std::string& typestr) {
	using Class = Rect<T>;
	py::class_<Class>(m, typestr.c_str())
		.def(py::init<>())
		.def_readwrite("left",   &Class::left)
		.def_readwrite("top",    &Class::top)
		.def_readwrite("right",  &Class::right)
		.def_readwrite("bottom", &Class::bottom)
		.def("Contains", py::overload_cast<T, T>(&Class::Contains, py::const_))
		.def("Contains", py::overload_cast<const Rect<T>&>(&Class::Contains, py::const_))
		.def("Intersects",       &Class::Intersects)
		.def("RIntersects",      &Class::RIntersects)
		.def("Translate",        &Class::Translate)
		.def("Area",             &Class::Area)
		.def("Margin",           &Class::Margin);
}
#endif
