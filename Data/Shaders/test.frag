#version 440 core

out vec4 color;

in VS_OUT_STRUCT
{
    vec4 color;
} vs_in ;

void main()
{
  color = normalize(vs_in.color);
}