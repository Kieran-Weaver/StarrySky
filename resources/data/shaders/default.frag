#version 330
in vec4 clr;
in vec3 texps;
out vec4 outColor;
uniform sampler2DArray tex;
void main(){
	outColor = clr*texture(tex,texps);
	if (outColor.a == 0.0){
		discard;
	}
} 
