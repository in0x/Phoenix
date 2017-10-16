#version 430 core

in vec3 position;
in vec3 normal;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;

out VS_OUT_STRUCT 
{
	vec3 view;
	vec3 normal;
} vs_out ; 

void main()
{
	vec4 pos_vs = viewTf * modelTf * vec4(position, 1.0);

	vs_out.normal = mat3(viewTf * modelTf) * normal;
	vs_out.view = pos_vs.xyz;
	
	gl_Position = projectionTf * pos_vs;
}