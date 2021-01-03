#include <pybind11/Core.hpp>
#include <pybind11/stl.h>
#include <core/CMap.hpp>
#include <core/MEState.hpp>
#include <core/MovingEntity.hpp>
#include <core/Timer.hpp>
void pydef_MEState(py::module& m, const std::string& typestr) {
	py::class_<MEState>(m, typestr.c_str())
		.def(py::init<>())
		.def_readwrite("hitbox",      &MEState::hitbox)
		.def_readwrite("xspeed",      &MEState::xspeed)
		.def_readwrite("yspeed",      &MEState::yspeed)
		.def_readwrite("atLeftWall",  &MEState::atLeftWall)
		.def_readwrite("atRightWall", &MEState::atRightWall)
		.def_readwrite("atFloor",     &MEState::atFloor)
		.def_readwrite("atCeiling",   &MEState::atCeiling)
		.def_readwrite("onOneWay",    &MEState::onOneWay)
		.def("newFrame",              &MEState::newFrame)
		.def("setPosition",           &MEState::setPosition)
		.def("centerX",               &MEState::centerX)
		.def("centerY",               &MEState::centerY)
		.def("width",                 &MEState::width)
		.def("height",                &MEState::height);
}

void pydef_MovingEntity(py::module& m, const std::string& typestr) {
	py::class_<MovingEntity>(m, typestr.c_str())
		.def(py::init<float, float>())
		.def(py::init<float, float, int, int>())
		.def_readwrite("maxFallSpeed",   &MovingEntity::maxFallSpeed)
		.def_readwrite("dropFromOneWay", &MovingEntity::dropFromOneWay)
		.def_readwrite("dead",           &MovingEntity::dead)
		.def("Update",                   &MovingEntity::Update)
		.def("getState",                 &MovingEntity::getState)
		.def("setSpeed",                 &MovingEntity::setSpeed)
		.def("setSize",                  &MovingEntity::setSize)
		.def("warpto",                   &MovingEntity::warpto);
}

void pydef_Timer(py::module& m, const std::string& typestr) {
	py::class_<Timer>(m, typestr.c_str())
		.def(py::init<>())
		.def("__call__", [](Timer& self) {
			return self();
		})
		.def_property("delay", &Timer::getDelay, &Timer::setDelay)
		.def_property("time",  &Timer::getTime,  &Timer::setTime);
}


void pydef_CMap(py::module& m, const std::string& typestr) {
	py::enum_<WallType>(m, "WallType")
		.value("LWALL",  WallType::LWALL)
		.value("RWALL",  WallType::RWALL)
		.value("CEIL",   WallType::CEIL)
		.value("FLOOR",  WallType::FLOOR)
		.value("ONEWAY", WallType::ONEWAY);

	py::class_<Surface>(m, "Surface")
		.def(py::init<>())
		.def_readwrite("hitbox", &Surface::hitbox)
		.def_readwrite("flags",  &Surface::flags);
	
	py::class_<CMap>(m, typestr.c_str())
		.def(py::init<>())
		.def("load",                       &CMap::load)
		.def("insert",                     &CMap::insert)
		.def("collide",                    &CMap::collide)
		.def_property_readonly("surfaces", &CMap::getSurfaces)
		.def_property_readonly("bounds",   &CMap::getbounds);
}
