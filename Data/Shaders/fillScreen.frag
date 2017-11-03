#version 430 core

uniform sampler2D fbTexColor;

in vec2 texCoord;
out vec4 color;

void main()
{
	color = texture(fbTexColor, texCoord);
}