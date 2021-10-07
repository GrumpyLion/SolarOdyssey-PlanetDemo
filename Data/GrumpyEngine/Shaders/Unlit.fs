#version 450 core

in VERTEX_OUT 
{
	vec2 myTexcoord;
} VertexIn;

layout (location = 0) out vec4 oColor;

uniform sampler2D uTexture;

void main()
{	
	oColor = texture(uTexture, VertexIn.myTexcoord);
	if (oColor.a < 0.5)
		discard;
}