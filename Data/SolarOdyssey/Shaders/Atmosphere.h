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