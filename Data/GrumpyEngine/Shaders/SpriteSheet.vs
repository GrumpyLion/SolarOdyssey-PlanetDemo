#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

out VERTEX_OUT 
{
	vec2 myTexcoord;
} VertexOut;

uniform mat4 uModelMatrix;
uniform vec2 uSpriteSheetSize;
uniform vec2 uSpriteSize;
uniform vec2 uSpritePosition;

void main()
{
	vec2 spriteLocation = (vec2(0.0, uSpriteSheetSize.y) - (uSpritePosition + vec2(0.0, uSpriteSize.y)) * vec2(-1.0, 1.0)) / uSpriteSheetSize;
	vec2 spriteSize = uSpriteSize / uSpriteSheetSize;

	VertexOut.myTexcoord = Texcoord * spriteSize + spriteLocation;
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrix * uModelMatrix * vec4(Position, 1.0);
}