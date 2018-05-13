#version 430 core

uniform sampler2D normalRGBRoughnessA_tex;
uniform sampler2D kDiffuseRGBDepthA_tex;
uniform sampler2D metallicR_tex;

const int MAX_DIR_LIGHTS = 32;

uniform vec3 dl_directionEye[MAX_DIR_LIGHTS];
uniform vec3 dl_color[MAX_DIR_LIGHTS];
uniform int  dl_numLights;

const int MAX_DIR_LIGHTS = 32;

uniform vec3 dl_directionEye[MAX_DIR_LIGHTS];
uniform vec3 dl_color[MAX_DIR_LIGHTS];
uniform int  dl_numLights;

in vec2 texCoord;
in vec4 rayEye;

out vec4 color;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float normalDistGGX(vec3 N, vec3 H, float roughness)
{
	float alpha2 = roughness * roughness;
	alpha2 = alpha2 * alpha2;
	
	float n_dot_h2 = max(dot(N, H), 0.0);
	n_dot_h2 = n_dot_h2 * n_dot_h2;
	
	float denom = (n_dot_h2 * (alpha2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return alpha2 / denom;
}

float geometrySchlickGGX(float n_dot_v, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	float denom = n_dot_v * (1.0 - k) + k;
	
	return n_dot_v / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float n_dot_v = max(dot(N, V), 0.0);
	float n_dot_l = max(dot(N, L), 0.0);
	
	float ggx1 = geometrySchlickGGX(n_dot_l, roughness);
	float ggx2 = geometrySchlickGGX(n_dot_v, roughness);
	
	return ggx1 * ggx2;
}

void main()
{
	vec4 diffsuseDepth = texture(kDiffuseRGBDepthA_tex, texCoord);
	vec4 normalRoughness = texture(normalRGBRoughnessA_tex, texCoord);
		
	vec3 kDiffuse = diffsuseDepth.xyz;
	float roughness = normalRoughness.w;
	float metallic = texture(metallicR_tex, texCoord).x;
	
	float zEye = diffsuseDepth.w;
	vec4 positionEye = rayEye * zEye;
	vec3 normalEye = normalRoughness.xyz;
	
	vec3 N = normalize(normalEye);
	vec3 V = normalize(-positionEye.xyz);
	
	vec3 lightOut = vec3(0.0);
	
	for (int i = 0; i < dl_numLights; i++)
	{
		vec3 L = normalize(-dl_directionEye[i]);
		vec3 H = normalize(V + L);
	
		float cosTheta = max(dot(H, V), 0.0);
		
		vec3 radiance = dl_color[i] * cosTheta;

		vec3 f0 = vec3(0.04);	
		f0 = mix(f0, kDiffuse, metallic);
		vec3 fresnel = fresnelSchlick(cosTheta, f0);
		
		float normalDist = normalDistGGX(N, H, roughness);
		float geometry = geometrySmith(N, V, L, roughness);
		
		float n_dot_l = max(dot(N, L), 0.0);
		
		vec3 numerator = normalDist * geometry * fresnel;
		float denominator = 4.0 * max(dot(N, V), 0.0) * n_dot_l;
		vec3 specularBRDF = numerator / max(denominator, 0.001);
		
		vec3 ks = fresnel;
		vec3 kd = vec3(1.0) - ks;
		
		kd *= 1.0 - metallic;
		
		lightOut += (kd * kDiffuse / PI + specularBRDF) * radiance * n_dot_l;
	}
	
	color = vec4(lightOut, 1.0);
}