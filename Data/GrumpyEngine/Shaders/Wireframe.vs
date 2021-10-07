#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;

uniform mat4 uModelMatrix;

void main()
{
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrix * uModelMatrix * vec4(Position, 1.0);
}