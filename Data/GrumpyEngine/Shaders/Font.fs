#version 450 core

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main()
{	
	oColor = texture(uTexture, iTexcoord);
	oColor.xyz *= uColor;
}