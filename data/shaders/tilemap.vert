#version 330
in vec2 position;
in vec2 Index;
uniform mat4 globalVP;
out vec2 index;
void main(){
	index = Index;
	gl_Position = globalVP*vec4(position,0.0,1.0);
}
