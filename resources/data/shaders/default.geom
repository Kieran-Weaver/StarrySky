#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in Vertex
{
	vec4 packedtexpos;
	vec4 packedmat2;
	vec4 color;
} in_data[];
out vec2 Texcoord;
out vec4 color;
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

	vec2 pos[4] = vec2[4](datamat * vec2(-0.5,0.5), datamat * vec2(-0.5, -0.5), datamat * vec2(0.5,0.5), datamat * vec2(0.5,-0.5));

	color = in_data[0].color;

	Texcoord = texlt + texwh.zy;
	gl_Position = globalVP * (gl_in[0].gl_Position + vec4(pos[0],0.0,0.0));
	EmitVertex();

	Texcoord = texlt;
	gl_Position = globalVP * (gl_in[0].gl_Position + vec4(pos[1],0.0,0.0));
	EmitVertex();
	
	Texcoord = texlt + texwh.xy;
	gl_Position = globalVP * (gl_in[0].gl_Position + vec4(pos[2],0.0,0.0));
	EmitVertex();

	Texcoord = texlt + texwh.xz;
	gl_Position = globalVP * (gl_in[0].gl_Position + vec4(pos[3],0.0,0.0));
	EmitVertex();
}
