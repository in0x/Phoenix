#version 440 core

out vec4 color;

in VS_OUT_STRUCT
{
    vec4 color;
} vs_in ;

void main()
{
  color = normalize(vs_in.color);
  //color = vec4(1.0, 0, 0, 1.0);
}