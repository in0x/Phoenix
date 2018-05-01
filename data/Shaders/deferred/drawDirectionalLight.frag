#version 430 core

uniform sampler2D normalRGBSpecExpA_tex;
uniform sampler2D kDiffuseRGBDepthA_tex;
uniform sampler2D kSpecularRGB_tex;

uniform vec3 lightDirectionEye;
uniform vec3 lightColor;

in vec2 texCoord;
in vec4 rayEye;

out vec4 color;

void main()
{
	vec4 diffsuseDepth = texture(kDiffuseRGBDepthA_tex, texCoord);
	vec4 normalSpecExp = texture(normalRGBSpecExpA_tex, texCoord);
	
	float zEye = diffsuseDepth.w;
	float specExp = normalSpecExp.w;
	
	vec4 positionEye = rayEye * zEye;
	vec3 normalEye = normalSpecExp.xyz;
	vec3 kDiffuse = diffsuseDepth.xyz;
	vec3 kSpecular = texture(kSpecularRGB_tex, texCoord).xyz;

	vec3 N = normalize(normalEye);
	vec3 V = normalize(-positionEye.xyz);
	vec3 L = normalize(-lightDirectionEye);
	vec3 H = normalize(V + L);
	
	float cosTh = max(dot(N, H), 0.0);
	float cosTi = max(dot(N, L), 0.0);
	
	color = vec4(((kDiffuse + kSpecular * pow(cosTh, specExp)) * lightColor * cosTi), 1.0);
}