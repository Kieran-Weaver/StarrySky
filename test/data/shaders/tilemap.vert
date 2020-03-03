#version 330
in uvec2 packedpos;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV;
	vec4 packedtileSize;
	vec4 tileData[256];
};
out uint index;
void main(){
	index = packedpos.y;
	vec2 position = vec2(packedpos.x & 0xFFFFu, packedpos.x / 65536u);
	position = vec2(position.x * packedtileSize.x, position.y * packedtileSize.y);
	mat2 tempMat = mat2(AffineV);
	gl_Position = vec4(tempMat*position + packedtileSize.zw, -1.0, 1.0);
}
