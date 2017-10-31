#version 430 core

in vec3 position;
in vec3 normals;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;

void main()
{
	gl_Position = projectionTf * viewTf * modelTf * vec4(position, 1.0);
}