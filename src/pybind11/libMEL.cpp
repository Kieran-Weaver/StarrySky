#include <pybind11/pybind11.h>
#include <pybind11/Rect.hpp>
namespace py = pybind11;

PYBIND11_MODULE(MEL, m) {
	m.doc() = R"pbdoc(
		Starry Sky Collision engine
	)pbdoc";
	
	pydef_Rect<float>(m,    "FloatRect");
	pydef_Rect<int>(m,      "IntRect");
	pydef_Rect<uint64_t>(m, "U64Rect");
}
