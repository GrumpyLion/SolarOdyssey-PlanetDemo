#version 450 core

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

uniform sampler2D uColor;
uniform sampler2D uMotionVectors;

void main()
{	
	vec2 velocity = -texture(uMotionVectors, iTexcoord).xy * 0.025;
	const float samples = 8;
	const float w = 1.0 / samples;
	
	for (int i = 0; i < samples; i++)
	{
		float t = i / (samples-1);
		oColor += texture(uColor, iTexcoord + velocity * t) * w;
	}
}