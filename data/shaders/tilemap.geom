#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in uint index[];
out vec2 texposition;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV;
	vec4 packedtileSize;
	vec4 tileData[256];
};
vec2 doTransform(vec2 initial, mat2 transform, vec2 position){
	return (transform * initial) + position; 
}
void main(){
	vec2 default_pos = gl_in[0].gl_Position.xy;
	mat2 AffineT = mat2(AffineV);
	vec2 texTopLeft = tileData[index[0]].xy;
	vec3 texSize = vec3(tileData[index[0]].zw, 0.0);
	texposition = texTopLeft + texSize.xz;
	vec2 halfTileSize = packedtileSize.xy / 2.0;
	gl_Position = globalVP * vec4(doTransform(vec2(halfTileSize.x,-halfTileSize.y), AffineT, default_pos),0.f,1.f);
	gl_Position.z = 1.0f;
	EmitVertex();
	texposition = texTopLeft;
	gl_Position = globalVP * vec4(doTransform(-halfTileSize, AffineT, default_pos),0.f,1.f);
	gl_Position.z = 1.0f;
	EmitVertex();
	texposition = texTopLeft + texSize.xy;
	gl_Position = globalVP * vec4(doTransform(halfTileSize, AffineT, default_pos),0.f,1.f);
	gl_Position.z = 1.0f;
	EmitVertex();
	texposition = texTopLeft + texSize.zy;
	gl_Position = globalVP * vec4(doTransform(vec2(-halfTileSize.x,halfTileSize.y), AffineT, default_pos),0.f,1.f);
	gl_Position.z = 1.0f;
	EmitVertex();
	EndPrimitive();
}
