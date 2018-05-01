#version 430 core

uniform sampler2D kDiffuseRGBDepthA_tex;

vec3 ambient = vec3(0.2, 0.2, 0.2);

in vec2 texCoord;

out vec4 color;

void main()
{
	vec4 diffsuseDepth = texture(kDiffuseRGBDepthA_tex, texCoord);
	color = vec4(diffsuseDepth.xyz * ambient, 1.0);
}