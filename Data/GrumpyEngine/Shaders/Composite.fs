#version 450 core

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

uniform sampler2D uColor;

void main()
{	
    oColor = texture(uColor, iTexcoord);
}