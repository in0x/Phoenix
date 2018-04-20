#version 430 core

vec2 data[4] = {
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0),
	vec2(1.0, 1.0),
	vec2(1.0, -1.0)
};

out vec2 texCoord;

void main()
{		
	float x = data[gl_VertexID].x;
	float y = data[gl_VertexID].y;
	
    texCoord.x = (x+1.0)*0.5;
    texCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
}