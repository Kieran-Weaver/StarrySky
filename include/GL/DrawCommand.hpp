#ifndef STARRYSKY_DC_HPP
#define STARRYSKY_DC_HPP
#include <optional>
#include <variant>
#include <array>
#include <vector>
#include <map>
#include <GL/Buffer.hpp>
#include <GL/Texture.hpp>
#include <GL/Program.hpp>
#include <GL/VertexArray.hpp>
#include <util/Rect.hpp>

namespace gl {
	enum class GLenum : unsigned int;
};

class ImDrawData;
template<typename T, size_t N>
using gl_attrib = std::optional<std::array<T, N>>;
template<typename T, size_t N>
using glMatrix = std::array<std::array<T, N>, N>;
struct gl_sfunc {
	gl::GLenum func;
	int ref;
	unsigned int mask;
};

namespace Draw{

	enum Primitive{
		Points,
		Lines,
		LineStrip,
		LineLoop,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	enum IdxType{
		Byte,
		Short,
		Int,
		None
	};

}

struct LoadCall{
	std::reference_wrapper<Buffer> buff;
	const void* data = nullptr;
	uint64_t size = 0;
	uint64_t position = 0;
};

struct DrawCall{
	Draw::Primitive type = Draw::Triangles;
	std::optional<Rect<int>> clip_rect;
	std::vector<Texture> textures = {};
	int32_t vtxOffset = 0;
	uint32_t vtxCount = 0;
	intptr_t idxOffset = 0;
	Draw::IdxType idxType = Draw::None;
	int32_t instances = -1;
	int32_t baseVertex = -1;
	std::function<void(const void*, const void*)> callback;
	std::array<const void*, 2> callback_opts;
};

struct DrawCommand{
	std::vector<LoadCall> bound_buffers;
	Program* program;
	VertexArray* VAO;
	std::optional<glMatrix<float, 4>> camera_override;
	std::vector<DrawCall> calls;
};

struct ConfCommand{
	std::map<gl::GLenum, bool>			 	enable_flags;
	gl_attrib<gl::GLenum, 2>				blend_func;
	std::optional<gl::GLenum>				blend_equation;
	std::optional<gl::GLenum> 				cull_face;
	std::optional<gl::GLenum> 				depth_func;
	gl_attrib<double, 2> 					depth_range;
	std::optional<gl::GLenum> 				logic_op;
	std::optional<float> 					line_width;
	gl_attrib<uint32_t, 2>					sample_mask;
	std::optional<gl_sfunc>					stencil_func;
	gl_attrib<gl::GLenum, 3>				stencil_op;
	std::optional<std::pair<float, bool>> 	sample_coverage;
	gl_attrib<float, 2> 					polygon_offset;
	std::optional<uint32_t> 				primitive_restart;
};

using DrawList = std::vector<std::variant<DrawCommand, ConfCommand>>;
#endif
