#version 430 core

uniform sampler2D kfinalRGBA_tex;

in vec2 texCoord;

out vec4 correctedColor;

const float gamma = 2.2;

vec3 toGamma(vec3 v) 
{
  return pow(v, vec3(1.0 / gamma));
}

vec4 toGamma(vec4 v) 
{
  return vec4(toGamma(v.rgb), v.a);
}

void main()
{
	vec3 hdrColor = texture(kfinalRGBA_tex, texCoord).xyz;
	
	// Reinhard tone mapping
    hdrColor = hdrColor / (hdrColor + vec3(1.0));
	
	// Gamma correction
    hdrColor = toGamma(hdrColor);	
	
	correctedColor = vec4(hdrColor, 1.0);
}