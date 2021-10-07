#version 450 core

#include "Data/GrumpyEngine/Shaders/Common.h"
#include "Data/SolarOdyssey/Shaders/Atmosphere.h"
#include "Data/SolarOdyssey/Shaders/Water.h"

layout (location = 0) in vec2 iTexcoord;
layout (location = 0) out vec4 oColor;

uniform sampler2D uColor;
uniform sampler2D uDepth;
uniform sampler2D uBlueNoise;
uniform sampler2D uAtmospherePrecompute;

uniform sampler2D uWaveNormalsA;
uniform sampler2D uWaveNormalsB;

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

/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
vec3 random3(vec3 c) 
{
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) 
{
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

vec3 hash33(vec3 q)
{
    uvec3 p = uvec3(ivec3(q));
    p = p*uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx*(p.zxy^(p >> 3U));
    return vec3(p^(p >> 16U))*(1.0/vec3(0xffffffffU));
}

vec3 stars(vec3 p)
{
    vec3 c = vec3(0.);
    float res = CameraBuffer.myResolutionAndTime.x*1.;
    
	for (float i=0.;i<4.;i++)
    {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        vec2 rn = hash33(id).xy;
        float c2 = 1.-smoothstep(0.,.6,length(q));
        c2 *= step(rn.x,.0005+i*i*0.001);
        c += c2*(mix(vec3(1.0,0.49,0.1),vec3(0.75,0.9,1.),rn.y)*0.1+0.9);
        p *= 1.3;
    }
    return c*c*.8;
}

// Reoriented Normal Mapping
// http://blog.selfshadow.com/publications/blending-in-detail/
// Altered to take normals (-1 to 1 ranges) rather than unsigned normal maps (0 to 1 ranges)
vec3 BlendRNM(vec3 n1, vec3 n2)
{
	n1.z += 1;
	n2.xy = -n2.xy;

	return n1 * dot(n1, n2) / n1.z - n2;
}

// Sample normal map with triplanar coordinates
// Returned normal will be in obj/world space (depending whether pos/normal are given in obj or world space)
// Based on: medium.com/@bgolus/normal-mapping-for-a-triplanar-shader-10bf39dca05a
vec3 TriplanarNormal(vec3 vertPos, vec3 normal, vec3 scale, vec2 offset, sampler2D normalMap)
{
	vec3 absNormal = abs(normal);

	// Calculate triplanar blend
	vec3 blendWeight = clamp(pow(normal, vec3(2.0)), vec3(0.0), vec3(1.0));
	// Divide blend weight by the sum of its components. This will make x + y + z = 1
	blendWeight /= dot(blendWeight, vec3(1.0));

	// Calculate triplanar coordinates
	vec2 uvX = vertPos.zy * scale.xy + offset;
	vec2 uvY = vertPos.xz * scale.xy + offset;
	vec2 uvZ = vertPos.xy * scale.xy + offset;

	// Sample tangent space normal maps
	// UnpackNormal puts values in range [-1, 1] (and accounts for DXT5nm compression)
	vec3 tangentNormalX = texture(normalMap, uvX).xyz * 2.0 - 1.0;
	vec3 tangentNormalY = texture(normalMap, uvY).xyz * 2.0 - 1.0;
	vec3 tangentNormalZ = texture(normalMap, uvZ).xyz * 2.0 - 1.0;

	// Swizzle normals to match tangent space and apply reoriented normal mapping blend
	tangentNormalX = BlendRNM(vec3(normal.zy, absNormal.x), tangentNormalX);
	tangentNormalY = BlendRNM(vec3(normal.xz, absNormal.y), tangentNormalY);
	tangentNormalZ = BlendRNM(vec3(normal.xy, absNormal.z), tangentNormalZ);

	// Apply input normal sign to tangent space Z
	vec3 axisSign = sign(normal);
	tangentNormalX.z *= axisSign.x;
	tangentNormalY.z *= axisSign.y;
	tangentNormalZ.z *= axisSign.z;

	// Swizzle tangent normals to match input normal and blend together
	vec3 outputNormal = normalize(
		tangentNormalX.zyx * blendWeight.x +
		tangentNormalY.xzy * blendWeight.y +
		tangentNormalZ.xyz * blendWeight.z
	);

	return outputNormal;
}

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
    //

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
    //

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
    //

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