#version 430 core

uniform sampler2D normalRGBRoughnessA_tex;
uniform sampler2D kDiffuseRGBDepthA_tex;
uniform sampler2D metallicR_tex;

const int MAX_DIR = 32;

const int MAX_POINT = 64;

const int MAX_SPOT = 64;

layout(std140, binding = 1) uniform LightDataBuffer
{
	vec3 dl_directionEye[MAX_DIR];
	vec3 dl_color[MAX_DIR];
	int  dl_numLights;
	
	vec4 pl_positionEyeRadius[MAX_POINT];
	vec4 pl_colorIntensity[MAX_POINT];
	int  pl_numLights;
} lights;

in vec2 texCoord;
in vec4 rayEye;

out vec4 color;

const float PI = 3.14159265359;

const float ambientFactor = 0.1;

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

vec3 shade(vec3 N, vec3 V, vec3 L, float metallic, float roughness, vec3 kDiffuse)
{
		vec3 H = normalize(V + L);
		float cosTheta = max(dot(H, V), 0.0);	
			
		vec3 f0 = vec3(0.04);	
		f0 = mix(f0, kDiffuse, metallic);
		vec3 fresnel = fresnelSchlick(cosTheta, f0);
		
		float normalDist = normalDistGGX(N, H, roughness);
		float geometry = geometrySmith(N, V, L, roughness);
		
		float n_dot_l = clamp(max(dot(N, L), 0.0), 0.0, 1.0);
		
		vec3 numerator = normalDist * geometry * fresnel;
		float denominator = 4.0 * max(dot(N, V), 0.0) * n_dot_l;
		vec3 specularBRDF = numerator / max(denominator, 0.001);
		
		vec3 ks = fresnel;
		vec3 kd = vec3(1.0) - ks;
		
		kd *= 1.0 - metallic;
		
		return (kd * kDiffuse / PI + specularBRDF) * n_dot_l;
}

vec3 shadeDirectionalLight(int lightIdx, vec3 N, vec3 V, float metallic, float roughness, vec3 kDiffuse)
{
	vec3 L = normalize(-lights.dl_directionEye[lightIdx]);
	
	vec3 lightColor = lights.dl_color[lightIdx];
	float intensity = length(lightColor);
	lightColor = normalize(lightColor);
	
	return shade(N, V, L, metallic, roughness, kDiffuse) * intensity * lightColor;	
}

float smoothDistanceAtten(float distSqr, float invSqrRadius)
{
	float factor = distSqr * invSqrRadius;
	float smoothFactor = clamp((1.0 - factor * factor), 0.0, 1.0);
	return smoothFactor * smoothFactor;
}

float distanceAtten(vec3 lightVector, float radius)
{
	float distSqr = dot(lightVector, lightVector);
	float invSqrRadius = 1.0 / (radius * radius);
	
	float attenuation = 1.0 / (max(distSqr, 0.01 * 0.01));
	attenuation *= smoothDistanceAtten(distSqr, invSqrRadius);
	
	return attenuation;
}

vec3 shadePointLight(int lightIdx, vec3 N, vec3 V, vec3 positionEye, float metallic, float roughness, vec3 kDiffuse)
{
	vec3 lightPosEye = lights.pl_positionEyeRadius[lightIdx].xyz;
	float radius = lights.pl_positionEyeRadius[lightIdx].w;

	vec3 L = lightPosEye - positionEye;
	float attenuation = distanceAtten(L, radius);
	L = normalize(L);
	
	vec3 lightColor = lights.pl_colorIntensity[lightIdx].xyz;
	float intensity = lights.pl_colorIntensity[lightIdx].w;
	
	return shade(N, V, L, metallic, roughness, kDiffuse) * ((intensity * normalize(lightColor)) / (4 * PI)) * attenuation;
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
	
	//lightOut += kDiffuse * ambientFactor;
	
	for (int i = 0; i < lights.dl_numLights; i++)
	{	
		lightOut += shadeDirectionalLight(i, N, V, metallic, roughness, kDiffuse);
	}
	
	for (int i = 0; i < lights.pl_numLights; i++)
	{
		lightOut += shadePointLight(i, N, V, positionEye.xyz, metallic, roughness, kDiffuse);
	}
	
	color = vec4(lightOut, 1.0);
}