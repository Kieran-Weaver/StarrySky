#version 330
in uvec2 packedpos;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 unusedVec;
	vec4 packedtSize;
};
out uint index;
void main(){
	index = packedpos.y;
	vec2 position = vec2(packedpos.x & 0xFFFFu, packedpos.x / 65536u);
	mat2 tempMat = mat2(unusedVec);
	gl_Position = vec4(tempMat*position*packedtSize.xy + packedtSize.zw, 0.0, 1.0);
}
