#version 330
in int packedpos;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 unusedVec;
	vec4 packedtSize;
};
out int index;
void main(){
	index = packedpos & 0x000000FF;
	vec2 position = vec2((packedpos >> 20), (packedpos & 0x000FFF00) >> 8);
	mat2 tempMat = mat2(unusedVec);
	gl_Position = vec4(tempMat*position*packedtSize.xy + packedtSize.zw, 0.0, 1.0);
}
