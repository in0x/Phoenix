#version 430 core

uniform sampler2D kfinalRGBA_tex;

in vec2 texCoord;

out vec4 correctedColor;

const float gamma = 2.2;

void main()
{
	vec4 finalColor = texture(kfinalRGBA_tex, texCoord);
	
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0/gamma));
	
	correctedColor = finalColor;
}