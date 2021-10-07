#version 450 core

layout(location=0) in vec3 Position;
layout(location=2) in vec2 Texcoord;

out VERTEX_OUT 
{
	vec3 myOrigin;
	vec3 myDirection;
	vec2 myTexcoord;
} VertexOut;

layout (std140, binding = 0) uniform GLOBAL_BUFFER
{ 
	mat4 myProjectionMatrix;
	mat4 myPrevProjectionMatrix;
	mat4 myViewMatrix;
	mat4 myPrevViewMatrix;
	mat4 myInvProjViewMatrix;
	vec4 myResolutionAndTime;
} CameraBuffer;

void main()
{
	gl_Position = vec4(Position, 1.0);
	VertexOut.myTexcoord = Texcoord;

	float near = 0.1;
	float far = 100.0;
	VertexOut.myOrigin = (CameraBuffer.myInvProjViewMatrix * vec4(Position.xy, -1.0, 1.0) * near).xyz;
	VertexOut.myDirection = (CameraBuffer.myInvProjViewMatrix * vec4(Position.xy * (far - near), far + near, far - near)).xyz;
	VertexOut.myDirection = normalize(VertexOut.myDirection);
}