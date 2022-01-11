#version 460 core

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

#include "Data/GrumpyEngine/Shaders/GridParams.h"
#include "Data/GrumpyEngine/Shaders/GridFunctions.h"
#include "Data/GrumpyEngine/Shaders/Common.h"

void main()
{	
	oColor = gridColor(iTexcoord);
}