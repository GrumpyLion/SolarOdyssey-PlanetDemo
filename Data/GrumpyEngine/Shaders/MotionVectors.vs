#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

out VERTEX_OUT 
{
	vec3 myVelocity;
} VertexOut;

uniform mat4 uModelMatrix;
uniform mat4 uPrevModelMatrix;

void main()
{
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrix * uModelMatrix * vec4(Position, 1.0);
	
	vec4 current = uModelMatrix * vec4(Position, 1.0);
	vec4 previous = uPrevModelMatrix * vec4(Position, 1.0);
	
	vec3 motionVector = current.xyz - previous.xyz;
	
	current = CameraBuffer.myViewMatrix * uModelMatrix * vec4(Position, 1.0);
	previous = CameraBuffer.myPrevViewMatrix * uPrevModelMatrix * vec4(Position, 1.0);
	
	vec3 ndcPos = current.xyz / current.w;
	vec3 ndcPosPrev = previous.xyz /= previous.w;
	
	VertexOut.myVelocity = ndcPos.xyz - ndcPosPrev.xyz;
}