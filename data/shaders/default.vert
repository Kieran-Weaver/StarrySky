#version 150
in vec2 position;
in vec2 texcoord;
out vec2 Texcoord;
layout (std140) uniform VP{
	mat4 globalVP;
};
void main(){
	Texcoord = texcoord;
	gl_Position = globalVP*vec4(position,0.0,1.0);
} 
