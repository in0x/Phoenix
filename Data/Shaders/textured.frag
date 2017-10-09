#version 430 core

out vec4 color;

uniform sampler2D tex;

in VS_OUT_STRUCT 
{
	vec2 texCoords;
} vs_in ; 

void main()
{
	vec4 texel = texture(tex, vs_in.texCoords);
	color = texel;
	//color = vec4(vs_in.texCoords, 0, 1) + texel;
}
