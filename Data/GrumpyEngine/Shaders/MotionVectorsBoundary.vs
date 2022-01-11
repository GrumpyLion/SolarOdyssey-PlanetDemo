#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

out VERTEX_OUT 
{
	vec3 myVelocity;
} VertexOut;

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

void main()
{
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrixCentered * vec4(Position, 1.0);

	vec4 current = CameraBuffer.myViewMatrixCentered * vec4(Position, 1.0);
	vec4 previous = CameraBuffer.myPrevViewMatrixCentered * vec4(Position, 1.0);
	
	vec3 ndcPos = current.xyz / current.w;
	vec3 ndcPosPrev = previous.xyz /= previous.w;
	
	VertexOut.myVelocity = ndcPos.xyz - ndcPosPrev.xyz;
}