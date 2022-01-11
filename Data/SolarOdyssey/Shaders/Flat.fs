#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

in VERTEX_OUT 
{
	flat vec3 myNormal;
	vec3 myFragPos;
	vec2 myTexcoord;
	vec4 myFragPosSpotLightSpace;
	vec4 myFragPosDirectionalLightSpace;
} VertexIn;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oNormals;
layout (location = 2) out vec4 oFragPos;

uniform sampler2D uTexture;

void main()
{	
	vec3 objectColor = texture(uTexture, VertexIn.myTexcoord).xyz;
	vec3 norm = normalize(VertexIn.myNormal);
	vec3 result = CalcDirectionalLight(LightBuffer.myDirectionalLight, norm, objectColor.xyz, VertexIn.myFragPosDirectionalLightSpace);

	for (int i = 0; i < LightBuffer.mySpotLightCount; ++i)
	{
		result += CalcSpotLight(LightBuffer.mySpotLights[i], norm, objectColor, VertexIn.myFragPos, VertexIn.myFragPosSpotLightSpace);
	}

	oColor = vec4(result, 1.0);
    oNormals = vec4(norm, 1.0);
    oFragPos = vec4(VertexIn.myFragPos, 1.0);
}