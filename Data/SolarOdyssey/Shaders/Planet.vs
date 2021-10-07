#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

out VERTEX_OUT 
{
	vec3 myNormal;
	vec3 myFragPos;
	vec2 myTexcoord;
    vec3 myPosition;
	vec4 myFragPosSpotLightSpace;
	vec4 myFragPosDirectionalLightSpace;
} VertexOut;

uniform mat4 uModelMatrix;

void main()
{
	VertexOut.myFragPos = vec3(uModelMatrix * vec4(Position, 1.0));
	VertexOut.myTexcoord = Texcoord;
    VertexOut.myPosition = Position;
	VertexOut.myNormal = mat3(transpose(inverse(uModelMatrix))) * Normal;
	VertexOut.myFragPosSpotLightSpace = SpotLightBuffer.myProjectionMatrix * SpotLightBuffer.myViewMatrix * vec4(VertexOut.myFragPos, 1.0);
	VertexOut.myFragPosDirectionalLightSpace = DirectionalLightBuffer.myProjectionMatrix * DirectionalLightBuffer.myViewMatrix * vec4(VertexOut.myFragPos, 1.0);
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrix * uModelMatrix * vec4(Position, 1.0);
}