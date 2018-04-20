#version 430 core

in vec4 positionEye;
in vec4 normalEye;
in vec2 uv;

// legacy
uniform vec3 kSpecular;
uniform float specularExp;

// new 
uniform sampler2D matDiffuseTex;

layout (location = 0) out vec4 normalRGBSpecExpA;
layout (location = 1) out vec4 kDiffuseRGBDepthA;
layout (location = 2) out vec3 kSpecularRGB;

void main()
{
	normalRGBSpecExpA.xyz = normalize(normalEye.xyz);
	normalRGBSpecExpA.w = specularExp;
	
	kDiffuseRGBDepthA.xyz = texture(matDiffuseTex, uv).xyz;
	kDiffuseRGBDepthA.w = positionEye.z;
	
	kSpecularRGB = kSpecular;
}