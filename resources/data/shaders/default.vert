#version 330
layout(location = 0) in vec3 texpos;
layout(location = 1) in vec3 vtxpos;
layout(location = 2) in vec4 color;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV;
	vec4 packedtileSize;
	vec4 metadata;
	uvec4 texData;
};
out vec4 clr;
out vec3 texps;
void main(){
	gl_Position = globalVP * vec4(vtxpos, 1.0);
	clr = color;
	texps = texpos;
}
