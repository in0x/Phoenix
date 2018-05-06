#version 430 core

uniform sampler2D matDiffuseTex;
uniform sampler2D matRoughnessTex;
uniform sampler2D matMetallicTex;
uniform sampler2D matNormalTex;

in VS_OUT
{
	mat3 tangentToViewTf;
	vec4 viewPosition;
	vec4 viewNormal;
	vec2 uv;
} fs_in;

layout (location = 0) out vec4 normalRGBSpecExpA;
layout (location = 1) out vec4 kDiffuseRGBDepthA;
layout (location = 2) out vec3 kSpecularRGB;

void main()
{
	//normalRGBSpecExpA.xyz = normalize(fs_in.viewNormal.xyz);
	
	vec3 normal = texture(matNormalTex, fs_in.uv).xyz;
	normal = normalize(normal * 2.0 - 1.0);
	normalRGBSpecExpA.xyz = normalize(fs_in.tangentToViewTf * normal);
	
	normalRGBSpecExpA.w = 1.0;
	
	kDiffuseRGBDepthA.xyz = texture(matDiffuseTex, fs_in.uv).xyz;
	kDiffuseRGBDepthA.w = fs_in.viewPosition.z;
	
	kDiffuseRGBDepthA.xyz += texture(matRoughnessTex, fs_in.uv).xyz * 0.000001;
	kDiffuseRGBDepthA.xyz += texture(matMetallicTex, fs_in.uv).xyz * 0.000001;
	kDiffuseRGBDepthA.xyz += texture(matNormalTex, fs_in.uv).xyz * 0.000001;
	
	kSpecularRGB = vec3(0.0, 0.0, 0.0);
}