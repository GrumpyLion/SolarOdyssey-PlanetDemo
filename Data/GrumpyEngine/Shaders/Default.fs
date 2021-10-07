#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

in VERTEX_OUT 
{
	vec3 myNormal;
	vec3 myFragPos;
	vec2 myTexcoord;
	vec4 myFragPosSpotLightSpace;
	vec4 myFragPosDirectionalLightSpace;
} VertexIn;

layout (location = 0) out vec4 oColor;

uniform sampler2D uTexture;
uniform sampler2D uSpotLightDepth;
uniform sampler2D uDirectionalLightDepth;

float CalcSpotLightShadow()
{
    vec3 lightDir = normalize(SpotLightBuffer.myLightPos - VertexIn.myFragPos);
    vec3 projCoords = VertexIn.myFragPosSpotLightSpace.xyz / VertexIn.myFragPosSpotLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uSpotLightDepth, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	float bias = max(0.001 * (1.0 - dot(VertexIn.myNormal, lightDir)), 0.0001);  
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 objectColor)
{
    vec3 lightDir = normalize(light.myPosition - VertexIn.myFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float theta = dot(lightDir, normalize(light.myDirection)); 
    float epsilon = (light.myCutOff - light.myOuterCutOff);
    float intensity = clamp((theta - light.myOuterCutOff) / epsilon, 0.0, 1.0);
	float distance = length(light.myPosition - VertexIn.myFragPos);
	float attenuation = clamp(light.myIntensity - distance * distance / (light.myDistance * light.myDistance), 0.0, 1.0); attenuation *= attenuation;
    vec3 diffuse = light.myColor.xyz * objectColor * diff * intensity * attenuation;
	return diffuse * (1.0 /*- CalcSpotLightShadow()*/);
}

float CalcDirectionalLightShadow(DirectionalLight light, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = VertexIn.myFragPosDirectionalLightSpace.xyz / VertexIn.myFragPosDirectionalLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uDirectionalLightDepth, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.001 * (1.0 - dot(normal, light.myDirection)), 0.0001);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	return shadow;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 objectColor)
{
	vec3 lightDir = normalize(-light.myDirection);
	float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.myColor.xyz * objectColor * diff;
	return diffuse * (1.0 - CalcDirectionalLightShadow(light, normal));
}

void main()
{	
	vec3 objectColor = texture(uTexture, VertexIn.myTexcoord).xyz;
	vec3 norm = normalize(VertexIn.myNormal);
	vec3 result = vec3(0.0);
	result += CalcDirectionalLight(LightBuffer.myDirectionalLight, norm, objectColor);
	result += LightBuffer.myAmbientColor  * objectColor;

	for (int i = 0; i < LightBuffer.mySpotLightCount; ++i)
	{
		result += CalcSpotLight(LightBuffer.mySpotLights[i], norm, objectColor);
	}

	oColor = vec4(result, 1.0);
}