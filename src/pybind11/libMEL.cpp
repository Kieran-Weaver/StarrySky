#include <pybind11/pybind11.h>
#include <pybind11/Core.hpp>
#include <pybind11/Rect.hpp>
#include <pybind11/RTree.hpp>
namespace py = pybind11;

PYBIND11_MODULE(MEL, m) {
	m.doc() = R"pbdoc(
		Starry Sky Collision engine
	)pbdoc";
	
	pydef_Rect<float>(m,    "FloatRect");
	pydef_Rect<int>(m,      "IntRect");
	pydef_Rect<uint64_t>(m, "U64Rect");

	pydef_RTree<float>(m,   "FloatRTree");
	pydef_RTree<int>(m,     "IntRTree");
	pydef_RTree<uint64_t>(m,"U64RTree");

	pydef_CMap(m,           "CMap");
	pydef_MEState(m,        "MEState");
	pydef_MovingEntity(m,   "MovingEntity");
	pydef_Timer(m,          "Timer");
}
