#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in Vertex
{
	vec4 packedtexpos;
	vec4 packedmat2;
	vec2 center;
} in_data[];
out vec2 Texcoord;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV; // 16 bytes
	vec4 packedtileSize; // 16 bytes
	vec4 tileData[256];
};
void main() {
	vec2 texlt = in_data[0].packedtexpos.xy;
	vec3 texwh = vec3(in_data[0].packedtexpos.zw, 0.0);
	mat2 datamat = mat2(in_data[0].packedmat2);

	vec2 pos = datamat * vec2(-0.5, -0.5);
	Texcoord = texlt;
	gl_Position = globalVP * vec4(in_data[0].center + pos,0.0,1.0);
	EmitVertex();
	
	Texcoord = texlt + texwh.xz;
	pos = datamat * vec2(0.5,-0.5);
	gl_Position = globalVP * vec4(in_data[0].center + pos,0.0,1.0);
	EmitVertex();
	
	Texcoord = texlt + texwh.zy;
	pos = datamat * vec2(-0.5,0.5);
	gl_Position = globalVP * vec4(in_data[0].center + pos,0.0,1.0);
	EmitVertex();

	Texcoord = texlt + texwh.xy;
	pos = datamat * vec2(0.5,0.5);
	gl_Position = globalVP * vec4(in_data[0].center + pos,0.0,1.0);
	EmitVertex();
}
