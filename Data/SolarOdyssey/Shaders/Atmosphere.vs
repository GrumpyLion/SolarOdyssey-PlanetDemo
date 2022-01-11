#version 450 core

layout(location=0) in vec3 Position;
layout(location=2) in vec2 Texcoord;

layout(location=0) out vec2 oTexcoord;

void main()
{
	gl_Position = vec4(Position, 1.0);
	oTexcoord = Texcoord;
}