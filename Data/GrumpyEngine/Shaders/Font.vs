#version 450 core

layout(location=0) in vec3 Position;
layout(location=2) in vec2 Texcoord;
layout(location=0) out vec2 oTexcoord;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{	
	gl_Position = uProjectionMatrix * uModelMatrix * vec4(Position, 1.0);
	oTexcoord = Texcoord;
}