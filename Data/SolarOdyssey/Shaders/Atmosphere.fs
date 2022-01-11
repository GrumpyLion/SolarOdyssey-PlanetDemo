#version 450 core

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

uniform sampler2D uColor;
uniform sampler2D uDepth;

uniform sampler2D uWaveNormalsA;
uniform sampler2D uWaveNormalsB;

#include "Data/GrumpyEngine/Shaders/Common.h"
#include "Data/SolarOdyssey/Shaders/Atmosphere.h"
#include "Data/SolarOdyssey/Shaders/GroundMapping.h"
#include "Data/SolarOdyssey/Shaders/Skybox.h"
#include "Data/SolarOdyssey/Shaders/Water.h"

void main()
{	
    vec4 color = texture(uColor, iTexcoord);
    float depth = texture(uDepth, iTexcoord).x;

    vec3 viewVector = vec3(CameraBuffer.myInvProjectionMatrix * vec4(iTexcoord * 2.0 - 1.0, 0, 1));
    viewVector = vec3(mat3(CameraBuffer.myInvViewMatrix) * viewVector);
    float linearDepth = LinearDepth(depth) * length(viewVector);
    vec3 rayDir = normalize(viewVector);
    vec3 rayOrigin = CameraBuffer.myCameraPos.xyz;

    //
    ////////////////////////////////////////////////////////////////////////
    // Water

    vec2 hitInfoOcean = RaySphere(AtmosphereBuffer.myPlanetCenter, AtmosphereBuffer.myPlanetRadius, rayOrigin, rayDir);
    float dstToOcean = hitInfoOcean.x;
    float oceanViewDepth = min(hitInfoOcean.y, linearDepth - dstToOcean);
    vec3 rayOceanIntersectPos = rayOrigin + rayDir * dstToOcean - AtmosphereBuffer.myPlanetCenter;

    if (oceanViewDepth > 0.0)
    {
        vec3 clipPlanePos = rayOrigin + viewVector * CameraBuffer.myNearFarFOV.x;
        float dstAboveWater = length(clipPlanePos - AtmosphereBuffer.myPlanetCenter) - AtmosphereBuffer.myPlanetRadius;
        const float t = 1.0 - exp(-oceanViewDepth * WaterBuffer.myDepthMultiplier);
        const float alpha =  1.0 - exp(-oceanViewDepth  * WaterBuffer.myAlphaMultiplier);
        vec4 oceanCol = mix(WaterBuffer.myWaterColor1, WaterBuffer.myWaterColor2, t);
        vec3 lightDir = -LightBuffer.myDirectionalLight.myDirection;
        vec3 oceanSphereNormal = normalize(rayOceanIntersectPos);

        const float time = CameraBuffer.myResolutionAndTime.z;
        const float waveSpeed = WaterBuffer.myWaveSpeed;
        const float waveNormalScale = WaterBuffer.myWaveNormalScale;
        vec2 waveOffsetA = vec2(time * waveSpeed, time * waveSpeed * 0.8);
        vec2 waveOffsetB = vec2(time * waveSpeed * - 0.8, time * waveSpeed * -0.3);
        vec3 waveNormal = TriplanarNormal(rayOceanIntersectPos, oceanSphereNormal, vec3(waveNormalScale), waveOffsetA, uWaveNormalsA);
        waveNormal = TriplanarNormal(rayOceanIntersectPos, waveNormal, vec3(waveNormalScale), waveOffsetB, uWaveNormalsB);
        waveNormal = normalize(mix(oceanSphereNormal, waveNormal, WaterBuffer.myWaveStrength));

        float diffuseLighting = clamp(dot(mix(waveNormal, oceanSphereNormal,0.75), lightDir), 0.0, 1.0);
        float specularAngle = acos(dot(normalize(lightDir - rayDir), waveNormal));
        float specularExponent = specularAngle / (1 - WaterBuffer.mySmoothness);
        float specularHighlight = exp(-specularExponent * specularExponent);

        oceanCol *= diffuseLighting + vec4(0.1);
        oceanCol += specularHighlight * float(dstAboveWater > 0) * WaterBuffer.mySpecularColor;

        oColor = color * (1-alpha) + oceanCol * alpha;
    }
    else
        oColor = color;

    //
    ////////////////////////////////////////////////////////////////////////
    // Atmosphere

    vec2 hitInfoAtmosphere = RaySphere(AtmosphereBuffer.myPlanetCenter, AtmosphereBuffer.myAtmosphereRadius, rayOrigin, rayDir);
    float dstToAtmosphere = hitInfoAtmosphere.x;
	float dstToSurface = min(linearDepth, dstToOcean);
    float dstThroughAtmosphere = min(hitInfoAtmosphere.y, dstToSurface - dstToAtmosphere);

    if (dstThroughAtmosphere > 0)
    {
        const float epsilon = 0.0001;
        vec3 pointInAtmosphere = rayOrigin + rayDir * (dstToAtmosphere + epsilon);
        vec3 light = CalculateLight(pointInAtmosphere, rayDir, dstThroughAtmosphere - epsilon * 2.0, oColor.xyz);
        oColor = vec4(light, 1.0);
    }

    //
    ////////////////////////////////////////////////////////////////////////
    // Space

    vec3 spaceColor = vec3(0.0);
    float noise = simplex3d(rayDir * .45 + CameraBuffer.myResolutionAndTime.z * 0.01) + 0.1;
    float noise2 = simplex3d(rayDir * .35 - CameraBuffer.myResolutionAndTime.z * 0.012) + 0.15;
    noise = clamp(noise, 0, 1);
    spaceColor = vec3(0.0, 0.075, 0.175);
    spaceColor = mix(spaceColor, vec3(0.1, 0.1, 0.35), noise);
    spaceColor = mix(spaceColor, vec3(0.35, 0.1, 0.35), noise2);

    vec3 sunDir = -LightBuffer.myDirectionalLight.myDirection;
    float sun = 1.0 - distance(normalize(rayDir), sunDir);
    sun = clamp(sun, 0.0f, 1.0f);

    float starStrength = clamp(((1.0 - pow(sun, 4.0))), 0.0, 1.0);
    float backgroundBrigthness = clamp(dot(oColor.xyz, vec3(2.)), 0.0, 1.0);
    starStrength *= 1.0 - backgroundBrigthness;
    spaceColor += stars(rayDir * 0.5) * starStrength * 1.5;

    float glow = sun;
    sun = pow(sun,12.0);
    sun = clamp(sun,0.0,1.0);
    
    glow = pow(glow,20.0);
    glow = clamp(glow,0.0,1.0);

    sun += glow;
    vec4 sunColor = LightBuffer.myDirectionalLight.myColor * sun;
    spaceColor += sunColor.xyz;

    if (depth >= 1.0)
        oColor += vec4(spaceColor, 1.0);
}