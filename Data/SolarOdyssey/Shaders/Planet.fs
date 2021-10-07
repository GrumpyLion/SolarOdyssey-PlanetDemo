#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"

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

uniform float uTextureScale;
uniform float uBlendSharpness;

uniform sampler2D uTexture;
uniform sampler2D uSpotLightDepth;
uniform sampler2D uDirectionalLightDepth;

float CalcSpotLightShadow()
{
    vec3 lightDir = normalize(SpotLightBuffer.myLightPos - VertexIn.myFragPos);
    vec3 projCoords = VertexIn.myFragPosSpotLightSpace.xyz / VertexIn.myFragPosSpotLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uSpotLightDepth, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.001 * (1.0 - dot(VertexIn.myNormal, lightDir)), 0.0001);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 objectColor)
{
    vec3 lightDir = normalize(light.myPosition - VertexIn.myFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float theta = dot(lightDir, normalize(light.myDirection)); 
    float epsilon = (light.myCutOff - light.myOuterCutOff);
    float intensity = clamp((theta - light.myOuterCutOff) / epsilon, 0.0, 1.0);
    float distance = length(light.myPosition - VertexIn.myFragPos);
    float attenuation = clamp(light.myIntensity - distance * distance / (light.myDistance * light.myDistance), 0.0, 1.0); attenuation *= attenuation;
    vec3 diffuse = light.myColor.xyz * objectColor * diff * intensity * attenuation;
    return diffuse * (1.0 /*- CalcSpotLightShadow()*/);
}

float CalcDirectionalLightShadow(DirectionalLight light, vec3 normal)
{
    // perform perspective divide
    vec3 projCoords = VertexIn.myFragPosDirectionalLightSpace.xyz / VertexIn.myFragPosDirectionalLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uDirectionalLightDepth, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.001 * (1.0 - dot(normal, light.myDirection)), 0.0001);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 objectColor)
{
    vec3 lightDir = normalize(-light.myDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = /*light.myColor.xyz **/ objectColor * diff;
    return diffuse * (1.0 - CalcDirectionalLightShadow(light, normal));
}

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
    vec3 result = vec3(0.0);
    result += CalcDirectionalLight(LightBuffer.myDirectionalLight, norm, objectColor.xyz);
    result += LightBuffer.myAmbientColor  * objectColor.xyz;

    for (int i = 0; i < LightBuffer.mySpotLightCount; ++i)
    {
    	result += CalcSpotLight(LightBuffer.mySpotLights[i], norm, objectColor.xyz);
    }

    oColor = vec4(result, objectColor.a);
}