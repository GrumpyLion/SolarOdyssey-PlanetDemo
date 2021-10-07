#version 450 core

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

out VERTEX_OUT 
{
	vec2 myTexcoord;
} VertexOut;

uniform mat4 uModelMatrix;

void main()
{
	VertexOut.myTexcoord = Texcoord;
	gl_Position =  vec4(Position, 1.0);
}