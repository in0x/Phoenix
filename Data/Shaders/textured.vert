#version 430 core

in vec3 position;
in vec2 uv;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;

out VS_OUT_STRUCT 
{
	vec2 texCoords;
} vs_out ; 

void main()
{
	vs_out.texCoords = uv;
	gl_Position = projectionTf * viewTf * modelTf * vec4(position, 1.0);
}