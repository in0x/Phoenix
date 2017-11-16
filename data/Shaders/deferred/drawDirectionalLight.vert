#version 430 core

vec4 frustumCorners[3] = {
vec4(-1.0f, -1.0f, 0.0f, 1.0f), 
vec4(3.0f, -1.0f, 0.0f, 1.0f),
vec4(-1.0f, 3.0f, 0.0f, 1.0f) 
};

uniform mat4 projectionTf;

out vec2 texCoord;
out vec4 rayEye;

void main()
{
	// Here I'm calculating the view space frustum corner ray. We pass this into
	// the fragment shader, which will give us an interpolated ray through our texel
	// , allowing us to sample the ray using the view space z to find x and y. This 
	// should not be in the shader, but instead passed in as a precalculated uniform array.
	// Since I don't have uniform arrays or constant buffers yet, I'm doing this in the shader
	// to verify that it works. We also normalize the ray to simplify the math.
	
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
	
	rayEye = inverse(projectionTf) * vec4(x, y, 0.0, 1.0); // ndc 
	rayEye /= rayEye.w; 
	rayEye /= rayEye.z; // view
	
    texCoord.x = (x+1.0)*0.5;
    texCoord.y = (y+1.0)*0.5;
    gl_Position = vec4(x, y, 0, 1);
}