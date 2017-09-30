#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 modelTf;
uniform mat4 viewTf;
uniform mat4 projectionTf;

out VS_OUT_STRUCT // Data Type
{
    vec4 color;
} vs_out ; // Instance name

void main()
{
  vs_out.color = vec4(1, 0, 0, 1);
  gl_Position = projectionTf * viewTf * modelTf * vec4(position,1);
}