#version 430 core

in vec4 positionEye;
in vec4 normalEye;

uniform vec3 kDiffuse;
uniform vec3 kSpecular;
uniform float specularExp;

layout (location = 0) out vec4 normalRGBSpecExpA;
layout (location = 1) out vec4 kDiffuseRGBDepthA;
layout (location = 2) out vec3 kSpecularRGB;

void main()
{
	normalRGBSpecExpA.xyz = normalize(normalEye.xyz);
	normalRGBSpecExpA.w = specularExp;
	
	kDiffuseRGBDepthA.xyz = kDiffuse;
	kDiffuseRGBDepthA.w = positionEye.z;
	
	kSpecularRGB = kSpecular;
}