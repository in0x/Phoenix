#version 430 core

in vec3 position;
in vec3 normal;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;
uniform vec3 lightPosition;

out VS_OUT_STRUCT 
{
	vec4 eyePosition;
	vec4 eyeNormal;
	vec4 eyeLight;
} vs_out ; 

void main()
{
	mat4 modelviewTf = viewTf * modelTf;
	
	vs_out.eyePosition = modelviewTf * vec4(position, 1.0);
	vs_out.eyeNormal = normalize(vec4(inverse(transpose(mat3(modelviewTf))) * normal, 0.0));
	vs_out.eyeLight = viewTf * vec4(lightPosition, 1.0);
	
	gl_Position = projectionTf * modelviewTf * vec4(position, 1.0);
}