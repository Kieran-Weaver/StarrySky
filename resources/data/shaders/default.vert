#version 330
layout(location = 0) in vec4 ptpos;
layout(location = 1) in vec4 pmat2;
layout(location = 2) in vec4 ctr;
layout(location = 3) in vec4 clr;
out Vertex
{
	vec4 packedtexpos;
	vec4 packedmat2;
	vec4 color;
} in_data;
void main(){
	gl_Position = ctr;
	in_data.packedmat2 = pmat2;
	in_data.packedtexpos = ptpos;
	in_data.color = clr;
} 
