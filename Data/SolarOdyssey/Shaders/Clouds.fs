#version 450 core
#include "Data/GrumpyEngine/Shaders/Common.h"

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

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
} AtmosphereBuffer;

uniform sampler2D uDepth;
uniform sampler2D uBlueNoise;

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
        float dstToSphereNear = max(0, (-b, - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

        if (dstToSphereFar >= 0)
        {
            return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }

    return vec2(0.0f, 0.0f);
}

float DensityAtPoint(vec3 samplePoint)
{
    float heightAboveSurface = length(samplePoint - AtmosphereBuffer.myPlanetCenter) - AtmosphereBuffer.myPlanetRadius;
    float height01 = heightAboveSurface / (AtmosphereBuffer.myAtmosphereRadius - AtmosphereBuffer.myPlanetRadius);
    float localDensity = exp(-height01 * AtmosphereBuffer.myDensityFalloff) * (1 - height01);
    return localDensity;
}

float OpticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength) // TODO optimize by baking this into a texture
{
    vec3 samplePoint = rayOrigin;
    const float numOpticalDepthPoints = AtmosphereBuffer.myNumOpticalDepthPoints;
    const float stepSize = rayLength / (numOpticalDepthPoints - 1);
    float opticalDepth = 0.0;

    for (int i = 0; i < numOpticalDepthPoints; ++i)
    {
        float localDensity = DensityAtPoint(samplePoint);
        opticalDepth += localDensity * stepSize;
        samplePoint += rayDir * stepSize;
    }
    return opticalDepth;
}

vec3 CalculateLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalCol)
{
    vec3 inScatterPoint = rayOrigin;
    const float numInScatterPoints = AtmosphereBuffer.myNumInScatterPoints;
    const float stepSize = rayLength / (numInScatterPoints - 1);
    const float atmosphereRadius = AtmosphereBuffer.myAtmosphereRadius;
    const vec3 planetCenter = AtmosphereBuffer.myPlanetCenter;
    const vec3 sunDir = -LightBuffer.myDirectionalLight.myDirection;
    vec3 inScatterLight = vec3(0.0);
    float viewRayOpticalDepth = 0.0;

    for (int i = 0; i < numInScatterPoints; ++i)
    {
        float sunRayLength = RaySphere(planetCenter, atmosphereRadius, inScatterPoint, sunDir).y;
        float sunRayOpticalDepth = OpticalDepth(inScatterPoint, sunDir, sunRayLength);
        viewRayOpticalDepth = OpticalDepth(inScatterPoint, -rayDir, stepSize * i);
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * AtmosphereBuffer.myScatterCoeffecients);
        float localDensity = DensityAtPoint(inScatterPoint);

        inScatterLight += localDensity * transmittance * AtmosphereBuffer.myScatterCoeffecients * stepSize;
        inScatterPoint += rayDir * stepSize;
    }

    float originalColTransmittance = exp(-viewRayOpticalDepth);
    return originalCol * originalColTransmittance + inScatterLight;
}

vec3 Hash33(vec3 q)
{
    uvec3 p = uvec3(ivec3(q));
    p = p*uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx*(p.zxy^(p >> 3U));
    return vec3(p^(p >> 16U))*(1.0/vec3(0xffffffffU));
}

vec3 Stars(vec3 p)
{
    vec3 c = vec3(0.);
    float res = CameraBuffer.myResolutionAndTime.x*1.;
    
	for (float i=0.;i<4.;i++)
    {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        vec2 rn = Hash33(id).xy;
        float c2 = 1.-smoothstep(0.,.6,length(q));
        c2 *= step(rn.x,.0005+i*i*0.001);
        c += c2*(mix(vec3(1.0,0.49,0.1),vec3(0.75,0.9,1.),rn.y)*0.1+0.9);
        p *= 1.3;
    }
    return c*c*.8;
}

mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );
float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float sdTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float fbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.12500*noise( p ); p = m*p*2.01;
    f += 0.06250*noise( p );
    return f;
}
/////////////////////////////////////

float stepUp(float t, float len, float smo)
{
  float tt = mod(t += smo, len);
  float stp = floor(t / len) - 1.0;
  return smoothstep(0.0, smo, tt) + stp;
}

float smin( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h); }

float map( in vec3 p )
{
    float iTime = CameraBuffer.myResolutionAndTime.z;
	vec3 q = p - vec3(0.0,0.5,1.0)*iTime*0.3;
    float f = fbm(q);
    float s1 = 1.0 - length(p + vec3(60, 0.0, 0.5)) * f + 2.0;
    // float s2 = 1.0 - length(p * vec3(0.1, 1.0, 0.2)) + f * 2.5;
    // float torus = 1. - sdTorus(p * 2.0, vec2(6.0, 0.005)) + f * 3.5;
    // float s3 = 1.0 - smin(smin(
    //                        length(p * 1.0 - vec3(cos(iTime * 3.0) * 6.0, sin(iTime * 2.0) * 5.0, 0.0)),
    //                        length(p * 2.0 - vec3(0.0, sin(iTime) * 4.0, cos(iTime * 2.0) * 3.0)), 4.0),
    //                        length(p * 3.0 - vec3(cos(iTime * 2.0) * 3.0, 0.0, sin(iTime * 3.3) * 7.0)), 4.0) + f * 2.5;
    
    // float t = mod(stepUp(iTime, 4.0, 1.0), 4.0);
    
	//float d = mix(s1, s2, clamp(t, 0.0, 1.0));
    float d = s1;
    //d = mix(d, torus, clamp(t - 1.0, 0.0, 1.0));
    //d = mix(d, s3, clamp(t - 2.0, 0.0, 1.0));
    //d = mix(d, s1, clamp(t - 3.0, 0.0, 1.0));
    
	return min(max(0.0, d), 1.0);
}

float jitter;

#define MAX_STEPS 32
#define SHADOW_STEPS 2
#define VOLUME_LENGTH 15.
#define SHADOW_LENGTH 2.

// Reference
// https://shaderbits.com/blog/creating-volumetric-ray-marcher
vec4 cloudMarch(vec3 p, vec3 ray)
{
    float density = 0.;

    float stepLength = VOLUME_LENGTH / float(MAX_STEPS);
    float shadowStepLength = SHADOW_LENGTH / float(SHADOW_STEPS);
    vec3 light = -LightBuffer.myDirectionalLight.myDirection;

    vec4 sum = vec4(0., 0., 0., 1.);
    
    vec3 pos = p + ray * jitter * stepLength;
    
    for (int i = 0; i < MAX_STEPS; i++)
    {
        if (sum.a < 0.1) {
        	break;
        }
        float d = map(pos);
    
        if( d > 0.001)
        {
            vec3 lpos = pos + light * jitter * shadowStepLength;
            float shadow = 0.;
    
            for (int s = 0; s < SHADOW_STEPS; s++)
            {
                lpos += light * shadowStepLength;
                float lsample = map(lpos);
                shadow += lsample;
            }
    
            density = clamp((d / float(MAX_STEPS)) * 20.0, 0.0, 1.0);
            float s = exp((-shadow / float(SHADOW_STEPS)) * 3.);
            sum.rgb += vec3(s * density) * vec3(1.1, 0.9, .5) * sum.a;
            sum.a *= 1.-density;

            sum.rgb += exp(-map(pos + vec3(0,0.25,0.0)) * .2) * density * vec3(0.15, 0.45, 1.1) * sum.a;
        }
        pos += ray * stepLength;
    }

    return sum;
}

void main()
{	
    vec4 color = texture(uColor, iTexcoord);
    float depth = texture(uDepth, iTexcoord).x;

    vec2 p = (iTexcoord.xy * 2.0 - CameraBuffer.myResolutionAndTime.xy) / min(CameraBuffer.myResolutionAndTime.x, CameraBuffer.myResolutionAndTime.y);
    float blueNoise = texture(uBlueNoise, iTexcoord * 2.0 + CameraBuffer.myResolutionAndTime.z).x;
    blueNoise = (blueNoise - 0.5) * 2.0;
    jitter = blueNoise;//hash(p.x + p.y * 57.0);

    vec4 camdir = CameraBuffer.myInvProjectionMatrix * vec4(-1.0 + 2.0 * iTexcoord, 1, 1);
    // camdir = camdir/camdir.w;//W normalized to get the point under the camera coordinate system
    vec3 rayDir = vec3(CameraBuffer.myInvViewMatrix * camdir);
    float linearDepth = LinearDepth(depth) * length(rayDir);
    rayDir = normalize(rayDir);

	vec3 rayOrigin = CameraBuffer.myCameraPos.xyz;
    vec2 hitInfo = RaySphere(AtmosphereBuffer.myPlanetCenter, AtmosphereBuffer.myAtmosphereRadius, rayOrigin, rayDir);
    float dstToAtmosphere = hitInfo.x;
    float dstThroughAtmosphere = min(hitInfo.y, linearDepth - dstToAtmosphere);
    // oColor = vec4(dstThroughAtmosphere / (atmoRadius * 2));

    if (dstThroughAtmosphere > 0)
    {
        const float epsilon = 0.0001;
        vec3 pointInAtmosphere = rayOrigin + rayDir * (dstToAtmosphere + epsilon);
        vec3 light = CalculateLight(pointInAtmosphere, rayDir, dstThroughAtmosphere - epsilon * 2, color.xyz);
        oColor = vec4(light, 1.0);
    }
    else
        oColor = color;

    vec4 col = cloudMarch(rayOrigin, rayDir);
    vec3 result = col.rgb + mix(vec3(0.3, 0.6, 1.0), vec3(0.05, 0.35, 1.0), p.y + 0.75) * (col.a);
    oColor += col;//vec4(result, 1.0);
}