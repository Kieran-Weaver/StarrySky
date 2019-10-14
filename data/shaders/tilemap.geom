#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in int index[];
out vec2 texposition;
layout (std140) uniform GSData{
	mat2 AffineT;
	int numTextures;
	vec4 tileData[256];
};
vec2 doTransform(vec2 initial, mat2 transform, vec2 position){
	return (transform * initial) + position; 
}
void main(){
	vec2 texTopLeft = tileData[index[0]].xy;
	vec3 texSize = vec3(tileData[index[0]].zw, 0.0);
	texposition = texTopLeft + texSize.xz;
	gl_Position = vec4(doTransform(vec2(0.5,-0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texTopLeft;
	gl_Position = vec4(doTransform(vec2(-0.5,-0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texTopLeft + texSize.xy;
	gl_Position = vec4(doTransform(vec2(0.5,0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	texposition = texTopLeft + texSize.zy;
	gl_Position = vec4(doTransform(vec2(-0.5,0.5), AffineT, gl_in[0].gl_Position.xy),0.f,1.f);
	EmitVertex();
	EndPrimitive();
}
