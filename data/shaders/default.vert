#version 150
in vec2 position;
in vec2 texcoord;
out vec2 Texcoord;
uniform mat4 VP;
void main(){
	Texcoord = texcoord;
	gl_Position = VP*vec4(position,0.0,1.0);
} 
