#version 450 core

in VERTEX_OUT 
{
	vec3 myVelocity;
} VertexIn;

layout (location = 0) out vec4 oColor;

void main()
{
	oColor = vec4(VertexIn.myVelocity, 1.0);
}