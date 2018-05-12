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

layout (location = 0) out vec4 normalRGBRoughnessA;
layout (location = 1) out vec4 kDiffuseRGBDepthA;
layout (location = 2) out vec3 metallicR;

void main()
{
	vec3 normal = texture(matNormalTex, fs_in.uv).xyz;
	normal = normalize(normal * 2.0 - 1.0);
	normal = fs_in.tangentToViewTf * normal;
	normal = normalize(normal + fs_in.viewNormal.xyz);
	
	normalRGBRoughnessA.xyz = normal;
	normalRGBRoughnessA.w = texture(matRoughnessTex, fs_in.uv).x;
	
	kDiffuseRGBDepthA.xyz = texture(matDiffuseTex, fs_in.uv).xyz;
	kDiffuseRGBDepthA.w = fs_in.viewPosition.z;
	
	metallicR = texture(matMetallicTex, fs_in.uv).xyz;
}