#version 430 core

out vec4 color;

in VS_OUT_STRUCT 
{
	vec4 eyePosition;
	vec4 eyeNormal;
	vec4 eyeLight;
} vs_in ; 


const vec3 modelColor = vec3(0.5, 0.0, 0.5);
const float shininess = 100;

//vec3 kAmbient = vec3(0.3, 0.3, 0.3);
//vec3 kDiffuse = vec3(0.7, 0.7, 0.7);
//vec3 kSpecular = vec3(0.5, 0.5, 0.5);

float kAmbient = 0.7;
float kDiffuse = 1.0;
float kSpecular = 0.5;

void main()
{
	vec3 L = normalize(vs_in.eyeLight.xyz - vs_in.eyePosition.xyz);
	vec3 N = normalize(vs_in.eyeNormal.xyz);
	float dotNL = max(dot(N, L), 0.0);
	
	vec3 R = reflect(-L, N);
	vec3 V = normalize(-vs_in.eyePosition.xyz); // Camera is at origin in view space.
	vec3 H = normalize(L + V);
	//float specular = pow(max(dot(normalize(R),normalize(V)), 0.0), shininess); 
	float specular = pow(max(dot(normalize(H),normalize(N)), 0.0), shininess); 
	
	color = vec4(kAmbient * modelColor + kDiffuse* dotNL * modelColor + kSpecular * specular * modelColor, 1.0);
	//color = vs_in.eyeNormal;
}
