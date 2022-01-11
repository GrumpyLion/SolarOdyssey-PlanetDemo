#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;

void main()
{
	gl_Position = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrixCentered * vec4(Position, 1.0);
}