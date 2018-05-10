#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec4 tangent; // Handedness in w, used to calc bitangent

uniform mat4 modelViewTf;
uniform mat3 normalTf;
uniform mat4 projectionTf;

out VS_OUT
{
	mat3 tangentToViewTf;
	vec4 viewPosition;
	vec4 viewNormal;
	vec2 uv;
} vs_out;

void main()
{
	vs_out.viewNormal = vec4(normalTf * normal, 0.0);
	vs_out.viewPosition = modelViewTf * vec4(position, 1.0);
	vs_out.uv = texcoord;
	
	vec3 bitangent = cross(normal, tangent.xyz) * tangent.w;
	
	vec3 viewT = normalize(normalTf * tangent.xyz);
	vec3 viewB = normalize(normalTf * bitangent);
	
	vs_out.tangentToViewTf = mat3(viewT, viewB, vs_out.viewNormal);
	
	gl_Position = projectionTf * vs_out.viewPosition;
}