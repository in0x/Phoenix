#version 430 core

out vec4 color;

uniform samplerCube cubemap;

in VS_OUT_STRUCT 
{
	vec3 view;
	vec3 normal;
} vs_in ; 

void main()
{
	vec3 r = reflect(vs_in.view, normalize(vs_in.normal));
	color = texture(cubemap, r);
}
