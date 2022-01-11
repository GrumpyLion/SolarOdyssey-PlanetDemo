#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

uniform mat4 uModelMatrix;
uniform sampler2D uTexture;
uniform sampler2D uNoise;

void main()
{
    vec3 displacedPosition = Position;
    if (texture(uTexture, Texcoord).x < 0.5)
    {
        vec4 worldPos = uModelMatrix * vec4(Position, 1.0);
        vec3 noise = texture(uNoise, worldPos.xy * 0.1  + CameraBuffer.myResolutionAndTime.z * 0.25).xyz;
        noise *= 2.0 - 1.0;
        displacedPosition += noise * 30.0;
    }
	gl_Position = DirectionalLightBuffer.myProjectionMatrix * DirectionalLightBuffer.myViewMatrix * uModelMatrix * vec4(displacedPosition, 1.0);
}