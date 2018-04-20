#version 430 core

uniform sampler2D normalRGBSpecExpA_tex;
uniform sampler2D kDiffuseRGBDepthA_tex;

uniform vec3 lightDirectionEye;
uniform vec3 lightColor;

in vec2 texCoord;
in vec4 rayEye;

layout (location = 0) out vec4 color;

void main()
{
	vec4 diffsuseDepth = texture(kDiffuseRGBDepthA_tex, texCoord);
	vec4 normalSpecExp = texture(normalRGBSpecExpA_tex, texCoord);
	
	float zEye = diffsuseDepth.w;
	
	vec4 positionEye = rayEye * zEye;
	vec3 normalEye = normalSpecExp.xyz;
	vec3 kDiffuse = diffsuseDepth.xyz;

	vec3 N = normalize(normalEye);
	vec3 L = normalize(-lightDirectionEye);
	
	float cosTi = max(dot(N, L), 0.0);

	color = vec4(kDiffuse * lightColor * cosTi, 1.0);
}