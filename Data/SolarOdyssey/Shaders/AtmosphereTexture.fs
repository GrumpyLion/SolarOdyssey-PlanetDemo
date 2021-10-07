#version 450 core

#include "Data/SolarOdyssey/Shaders/Atmosphere.h"

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

float DensityAtPoint(vec2 densitySamplePoint)
{
    float planetRadius = 1;
    vec2 planetCentre = vec2(0.0);
    float heightAboveSurface = length(densitySamplePoint - planetCentre) - planetRadius;
    float height01 = heightAboveSurface / (AtmosphereBuffer.myAtmosphereRadius - planetRadius);
    float localDensity = exp(-height01 * AtmosphereBuffer.myDensityFalloff) * (1 - height01);
    return localDensity;
}

float OpticalDepth(vec2 rayOrigin, vec2 rayDir, float rayLength)
{
    float numOpticalDepthPoints = AtmosphereBuffer.myNumOpticalDepthPoints;
    vec2 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / (numOpticalDepthPoints - 1);
    float OpticalDepth = 0;

    for (int i = 0; i < numOpticalDepthPoints; i++)
    {
        float localDensity = DensityAtPoint(densitySamplePoint);
        OpticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }
    return OpticalDepth;
}

float CalculateOutScattering(vec2 inPoint, vec2 outPoint)
{
    float planetRadius = 1;
    float skinWidth = planetRadius / 1000.0;
    float lightTravelDst = length(outPoint - inPoint);
    vec2 outScatterPoint = inPoint;
    vec2 rayDir = (outPoint - inPoint) / lightTravelDst;
    float stepSize = (lightTravelDst - skinWidth) / (AtmosphereBuffer.myNumOpticalDepthPoints);
    float outScatterAmount = 0;

    for (int i = 0; i < AtmosphereBuffer.myNumOpticalDepthPoints; i++)
    {
        outScatterPoint += rayDir * stepSize;
        // height at planet surface = 0, at furthest extent of atmosphere = 1
        float height = length(outScatterPoint - 0) - planetRadius;
        float height01 = clamp(height / (AtmosphereBuffer.myAtmosphereRadius - planetRadius), 0.0, 1.0);
        outScatterAmount += exp(-height01 * AtmosphereBuffer.myDensityFalloff) * stepSize;
    }
    return outScatterAmount;
}

void main()
{
    const float pi = 3.14159265359;
    const vec2 uv = iTexcoord;

    float height01 = uv.y;
    float angle = uv.x * pi;
    vec2 dir = vec2(sin(angle), cos(angle));
    float y = -2 * uv.x + 1;
    float x = sin(acos(y));
    dir = vec2(x,y);

    const float atmosphereRadius = AtmosphereBuffer.myAtmosphereRadius;
    const float planetRadius = AtmosphereBuffer.myPlanetRadius;
    vec2 inPoint = vec2(0, mix(planetRadius, atmosphereRadius, height01));
    float dstThroughAtmosphere = RaySphere(vec3(0), atmosphereRadius, vec3(inPoint,0), vec3(dir,0)).y;
    vec2 outPoint = inPoint + dir * RaySphere(vec3(0), atmosphereRadius, vec3(inPoint,0), vec3(dir,0)).y;
    float outScattering = OpticalDepth(inPoint + dir, dir, dstThroughAtmosphere);
    oColor = vec4(outScattering);
}