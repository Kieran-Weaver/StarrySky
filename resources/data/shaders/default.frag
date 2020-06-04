#version 330
in vec2 Texcoord;
in vec4 color;
out vec4 outColor;
uniform sampler2D tex;
void main(){
	outColor = color * texture(tex,Texcoord);
	if (outColor.a == 0.0){
		discard;
	}
} 
