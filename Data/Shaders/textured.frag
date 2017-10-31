#version 430 core

out vec4 color;

uniform sampler2D testTexture;

in VS_OUT_STRUCT 
{
	vec2 texCoords;
} vs_in ; 

void main()
{
	vec4 texel = texture(testTexture, vs_in.texCoords);
	color = texel;
}