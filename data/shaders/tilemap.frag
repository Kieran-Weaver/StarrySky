#version 330
in vec2 texcoord;
out vec4 outColor;
uniform sampler2D texPalette;
void main(){
	outColor = texture(texPalette, texcoord);
}