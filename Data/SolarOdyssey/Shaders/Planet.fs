#version 450 core

in VERTEX_OUT 
{
    vec3 myNormal;
    vec3 myFragPos;
    vec2 myTexcoord;
    vec3 myPosition;
    vec4 myFragPosSpotLightSpace;
    vec4 myFragPosDirectionalLightSpace;
} VertexIn;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oNormals;
layout (location = 2) out vec4 oFragPos;

uniform float uTextureScale;
uniform float uBlendSharpness;
uniform sampler2D uTexture;

#include "Data/GrumpyEngine/Shaders/Common.h"
#include "Data/SolarOdyssey/Shaders/GroundMapping.h"

void main()
{	
    vec2 uvX = VertexIn.myPosition.zy * uTextureScale;
    vec2 uvY = VertexIn.myPosition.xz * uTextureScale;
    vec2 uvZ = VertexIn.myPosition.xy * uTextureScale;

    vec4 colX = texture(uTexture, uvX);
    vec4 colY = texture(uTexture, uvY);
    vec4 colZ = texture(uTexture, uvZ);

    vec3 blendWeight = pow(abs(VertexIn.myNormal), vec3(uBlendSharpness));
    blendWeight /= dot(blendWeight, vec3(1.0));

    vec4 objectColor = colX * blendWeight.x + colY * blendWeight.y + colZ * blendWeight.z;
    
    // float distanceToCore = distance(VertexIn.myPosition.xyz, vec3(0.0));
    // objectColor = mix(vec4(0,0,1,1), objectColor, clamp((distanceToCore - 200.0f) * 5.0f, 0.0, 1.0));
    
    vec3 norm = normalize(VertexIn.myNormal);
    vec3 result = CalcDirectionalLight(LightBuffer.myDirectionalLight, norm, objectColor.xyz, VertexIn.myFragPosDirectionalLightSpace);

    for (int i = 0; i < LightBuffer.mySpotLightCount; ++i)
    {
    	result += CalcSpotLight(LightBuffer.mySpotLights[i], norm, objectColor.xyz, VertexIn.myFragPos, VertexIn.myFragPosSpotLightSpace);
    }

    oColor = vec4(result, objectColor.a);
    oNormals = vec4(norm, 1.0);
    oFragPos = vec4(VertexIn.myFragPos, 1.0);
}