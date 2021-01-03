#ifndef PYBIND11_CORE_HPP
#define PYBIND11_CORE_HPP
#include <pybind11/pybind11.h>
namespace py = pybind11;
void pydef_MEState(py::module& m, const std::string& typestr);
void pydef_MovingEntity(py::module& m, const std::string& typestr);
void pydef_Timer(py::module& m, const std::string& typestr);
void pydef_CMap(py::module& m, const std::string& typestr);
#endif
