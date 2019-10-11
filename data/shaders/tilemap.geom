#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in vec2 index[];
uniform mat2 AffineT;
const float tileW = 1.0/16.0;
const float tileH = 1.0/16.0;
const float halfTileW = 1.0/32.0;
const float halfTileH = 1.0/32.0;
out vec2 texposition;
vec2 doTransform(vec2 initial, mat2 transform, vec2 position){
	return (transform * initial) + position; 
}
void main(){
	vec2 texcenter = vec2(tileW * index[0].x,tileH * index[0].y);
	texposition = texcenter + vec2(halfTileW, -halfTileH);
	gl_Position = vec4(doTransform(vec2(0.5,-0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texcenter + vec2(-halfTileW, -halfTileH);
	gl_Position = vec4(doTransform(vec2(-0.5,-0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texcenter + vec2(halfTileW, halfTileH);
	gl_Position = vec4(doTransform(vec2(0.5,0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texcenter + vec2(-halfTileW, halfTileH);
	gl_Position = vec4(doTransform(vec2(-0.5,0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	EndPrimitive();
}