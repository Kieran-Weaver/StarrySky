#version 150
in vec4 ptpos;
in vec4 pmat2;
in vec2 ctr;
out Vertex
{
	vec4 packedtexpos;
	vec4 packedmat2;
	vec2 center;
} in_data;
void main(){
	in_data.center = ctr;
	in_data.packedmat2 = pmat2;
	in_data.packedtexpos = ptpos;
	gl_Position = vec4(ctr, 0.0, 1.0);
} 
