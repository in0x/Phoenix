#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;

out vec4 positionEye;
out vec4 normalEye;

void main()
{
	mat4 modelViewTf = viewTf * modelTf;
	normalEye = vec4(inverse(transpose(mat3(modelViewTf))) * normal, 0.0);
	positionEye = modelViewTf * vec4(position, 1.0);
	
	gl_Position = projectionTf * positionEye;
}