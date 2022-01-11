layout (std140, binding = 5) uniform ATMOSPHERE_BUFFER
{ 
    float myDensityFalloff;
    float myNumInScatterPoints;
    float myNumOpticalDepthPoints;
    float myPlanetRadius;
    float myAtmosphereHeight;
    float myAtmosphereRadius;
    vec3 myPlanetCenter;
    float myScatteringStrength;
    vec3 myScatterCoeffecients;
    float myIntensity;
    float myDitherStrength;
    float myDitherScale;
} AtmosphereBuffer;

uniform sampler2D uBlueNoise;
uniform sampler2D uAtmospherePrecompute;

vec2 RaySphere(vec3 center, float radius, vec3 rayOrigin, vec3 rayDir)
{
    vec3 offset = rayOrigin - center;
    float a = dot(rayDir, rayDir);
    float b = 2 * dot(offset, rayDir);
    float c = dot(offset, offset) - radius * radius;
    float d = b * b - 4 * a * c;

    if (d > 0)
    {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        if (dstToSphereFar >= 0)
        {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }

    return vec2(10000000.0f, 0.0f);
}

float DensityAtPoint(vec3 samplePoint)
{
    float heightAboveSurface = length(samplePoint - AtmosphereBuffer.myPlanetCenter) - AtmosphereBuffer.myPlanetRadius;
    float height01 = heightAboveSurface / (AtmosphereBuffer.myAtmosphereRadius - AtmosphereBuffer.myPlanetRadius);
    float localDensity = exp(-height01 * AtmosphereBuffer.myDensityFalloff) * (1.0 - height01);
    return localDensity;
}

float OpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
    vec3 samplePoint = rayOrigin;
    const float numOpticalDepthPoints = AtmosphereBuffer.myNumOpticalDepthPoints;
    const float stepSize = rayLength / (numOpticalDepthPoints - 1.0);
    float opticalDepth = 0.0;

    for (int i = 0; i < numOpticalDepthPoints; ++i)
    {
        float localDensity = DensityAtPoint(samplePoint);
        opticalDepth += localDensity * stepSize;
        samplePoint += rayDir * stepSize;
    }
    return opticalDepth;
}

float OpticalDepthBaked(vec3 rayOrigin, vec3 rayDir)
{
    float height = length(rayOrigin - AtmosphereBuffer.myPlanetCenter) - AtmosphereBuffer.myPlanetRadius;
    float height01 = clamp(height / (AtmosphereBuffer.myAtmosphereRadius - AtmosphereBuffer.myPlanetRadius), 0.0, 1.0);

    float uvX = 1 - (dot(normalize(rayOrigin - AtmosphereBuffer.myPlanetCenter), rayDir) * .5 + .5);
    return texture(uAtmospherePrecompute, vec2(uvX, height01)).x;
}

float OpticalDepthBaked2(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
    vec3 endPoint = rayOrigin + rayDir * rayLength;
    float d = dot(rayDir, normalize(rayOrigin - AtmosphereBuffer.myPlanetCenter));
    float opticalDepth = 0;

    const float blendStrength = 1.5;
    float w = clamp(d * blendStrength + .5, 0.0, 1.0);

    float d1 = OpticalDepthBaked(rayOrigin, rayDir) - OpticalDepthBaked(endPoint, rayDir);
    float d2 = OpticalDepthBaked(endPoint, -rayDir) - OpticalDepthBaked(rayOrigin, -rayDir);

    opticalDepth = mix(d2, d1, w);
    return opticalDepth;
}

vec2 SquareUV(vec2 uv)
{
	float width = CameraBuffer.myResolutionAndTime.x;
	float height = CameraBuffer.myResolutionAndTime.y;
	//float minDim = min(width, height);
	float scale = 1000;
	float x = uv.x * width;
	float y = uv.y * height;
	return vec2 (x/scale, y/scale);
}

vec3 CalculateLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalCol)
{
    float blueNoise = texture(uBlueNoise, SquareUV(iTexcoord) * AtmosphereBuffer.myDitherScale).x;
    blueNoise = (blueNoise - 0.5) * AtmosphereBuffer.myDitherStrength;

    vec3 inScatterPoint = rayOrigin;
    const float numInScatterPoints = AtmosphereBuffer.myNumInScatterPoints;
    float stepSize = rayLength / (numInScatterPoints - 1);
    const float atmosphereRadius = AtmosphereBuffer.myAtmosphereRadius;
    const vec3 planetCenter = AtmosphereBuffer.myPlanetCenter;
    const vec3 sunDir = -LightBuffer.myDirectionalLight.myDirection;
    const float intensity = AtmosphereBuffer.myIntensity;
    vec3 inScatterLight = vec3(0.0);
    float viewRayOpticalDepth = 0.0;

    for (int i = 0; i < numInScatterPoints; ++i)
    {
        float sunRayLength = RaySphere(planetCenter, atmosphereRadius, inScatterPoint, sunDir).y;
        float sunRayOpticalDepth = OpticalDepthBaked(inScatterPoint + sunDir * AtmosphereBuffer.myDitherStrength, sunDir);
        float localDensity = DensityAtPoint(inScatterPoint);
        viewRayOpticalDepth = OpticalDepthBaked2(rayOrigin, rayDir, stepSize * i);
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * AtmosphereBuffer.myScatterCoeffecients);

        inScatterLight += localDensity * transmittance;
        inScatterPoint += rayDir * stepSize;
    }
    inScatterLight *= AtmosphereBuffer.myScatterCoeffecients * intensity * stepSize / AtmosphereBuffer.myPlanetRadius;
    inScatterLight += blueNoise * 0.01;
    inScatterLight = max(inScatterLight, vec3(0.0));

    const float brightnessAdaptionStrength = 0.35;
    const float reflectedLightOutScatterStrength = 0.01;
    float brightnessAdaption = dot (inScatterLight,vec3(1)) * brightnessAdaptionStrength;
    float brightnessSum = viewRayOpticalDepth * intensity * reflectedLightOutScatterStrength + brightnessAdaption;
    float reflectedLightStrength = exp(-brightnessSum);
    float hdrStrength = clamp(dot(originalCol,vec3(1))/3-1, 0.0, 1.0);
    reflectedLightStrength = mix(reflectedLightStrength, 1, hdrStrength);
    vec3 reflectedLight = originalCol * reflectedLightStrength;
    
    return reflectedLight + inScatterLight;
    // float originalColTransmittance = exp(-viewRayOpticalDepth);
    // return originalCol * originalColTransmittance + inScatterLight;
}