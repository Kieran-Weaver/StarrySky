#version 330
in vec2 position;
in int Index;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 unusedVec;
	vec2 tSize;
};
out int index;
void main(){
	index = Index;
	mat2 tempMat = mat2(unusedVec);
	gl_Position = vec4(tempMat*position*tSize,0.0,1.0);
}
