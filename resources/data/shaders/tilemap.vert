#version 330
in uint xpos;
layout (std140) uniform VP{
	mat4 globalVP;
	vec4 AffineV;
	vec4 packedtileSize;
	vec4 metadata;
	uvec4 texData;
};
uniform samplerBuffer tBuffer;
uniform usamplerBuffer tTexture;
out vec2 texposition;
void main(){
	mat2 AffineT = mat2(AffineV);
	uint vert = uint(mod(xpos, 4));
	const vec2 positions[4] = vec2[4](vec2(-0.5, 0.5), vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5));

	vec2 position = positions[vert];
	position = vec2(position.x + xpos / 4u, position.y + gl_InstanceID);
	position = vec2(position.x * packedtileSize.x, position.y * packedtileSize.y);
//	gl_Position = vec4(position, 0.0, 0.0);
	position = AffineT * position + packedtileSize.zw;
	gl_Position = globalVP * vec4(position, 0.0, 1.0);
	gl_Position.z = metadata.x;
	
	int idx = int(texData.x) * int(gl_InstanceID);
	idx = idx + int(xpos)/4;
	uvec4 index = texelFetch(tTexture, idx);
	vec4 texRect = texelFetch(tBuffer, int(index.r));
	
	float xpos = texRect.x + uint(vert > 1u) * texRect.z;
	float ypos = texRect.y + uint((vert == 0u) || (vert == 3u)) * texRect.w;
	texposition = vec2(xpos, ypos);
}
