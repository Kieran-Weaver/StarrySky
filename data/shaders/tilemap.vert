#version 330
in uint _dummy;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV;
	vec4 packedtileSize;
	vec4 metadata;
	uvec4 texData;
};
out int instanceID;
void main(){
	instanceID = gl_InstanceID;
	vec2 position = vec2(uint(gl_InstanceID) % uint(texData.x), uint(gl_InstanceID) / uint(texData.x));
	position = vec2(position.x * packedtileSize.x, position.y * packedtileSize.y);
	mat2 tempMat = mat2(AffineV);
	gl_Position = vec4(tempMat*position + packedtileSize.zw, 0.0, 1.0);
}
