#version 460 core

layout(location=0) out vec2 oTexcoord;

#include "Data/GrumpyEngine/Shaders/GridParams.h"
#include "Data/GrumpyEngine/Shaders/Common.h"

void main()
{
	mat4 MVP = CameraBuffer.myProjectionMatrix * CameraBuffer.myViewMatrix;

	int idx = indices[gl_VertexID];
	vec3 position = pos[idx] * gridSize;

	gl_Position = MVP * vec4(position, 1.0);
	oTexcoord = position.xz;
}