#version 430 core

uniform sampler2D kfinalRGBA_tex;

in vec2 texCoord;

out vec4 correctedColor;

const float gamma = 2.2;

void main()
{
	vec3 hdrColor = texture(kfinalRGBA_tex, texCoord).xyz;
	
	// Reinhard tone mapping
    hdrColor = hdrColor / (hdrColor + vec3(1.0));
	
	// Gamma correction
    hdrColor = pow(hdrColor, vec3(1.0/gamma));
	
	correctedColor = vec4(hdrColor, 1.0);
}