struct DirectionalLight
{
	vec4 myColor;
	vec3 myDirection;
};

struct SpotLight
{
	vec4 myColor;
	vec3 myPosition;
	vec3 myDirection;
	float myCutOff;
	float myOuterCutOff;
	float myDistance;
	float myIntensity;
};

layout (std140, binding = 0) uniform CAMERA_BUFFER
{ 
	mat4 myProjectionMatrix;
	mat4 myPrevProjectionMatrix;
	mat4 myViewMatrix;
    mat4 myViewMatrixCentered;
	mat4 myPrevViewMatrix;
    mat4 myPrevViewMatrixCentered;
	mat4 myInvProjViewMatrix;
    mat4 myInvProjectionMatrix;
    mat4 myInvViewMatrix;
	vec4 myResolutionAndTime;
	vec4 myCameraPos;
    vec4 myCameraDir;
    vec4 myNearFarFOV;
} CameraBuffer;

layout (std140, binding = 1) uniform LIGHT_BUFFER
{ 
	DirectionalLight myDirectionalLight;
	vec3 myAmbientColor;
	int mySpotLightCount;
	SpotLight mySpotLights[16];
} LightBuffer;

layout (std140, binding = 11) uniform SPOT_LIGHT_SHADOW_BUFFER
{ 
	mat4 myProjectionMatrix;
	mat4 myViewMatrix;
	vec3 myLightPos;
	float myFarPlane;
} SpotLightBuffer;

layout (std140, binding = 12) uniform DIRECTIONAL_LIGHT_SHADOW_BUFFER
{ 
	mat4 myProjectionMatrix;
	mat4 myViewMatrix;
} DirectionalLightBuffer;

uniform sampler2D uSpotLightDepth;
uniform sampler2D uDirectionalLightDepth;

float LinearDepth(float depthSample)
{
    const float near = CameraBuffer.myNearFarFOV.x;
    const float far = CameraBuffer.myNearFarFOV.y;
    return near * far / (far + depthSample * (near - far));
}

float CalcSpotLightShadow(vec3 normal, vec3 fragPos, vec4 fragPosSpotLightSpace)
{
    vec3 lightDir = normalize(SpotLightBuffer.myLightPos - fragPos);
    vec3 projCoords = fragPosSpotLightSpace.xyz / fragPosSpotLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uSpotLightDepth, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 objectColor, vec3 fragPos, vec4 fragPosSpotLightSpace)
{
    vec3 lightDir = normalize(light.myPosition - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float theta = dot(lightDir, normalize(light.myDirection)); 
    float epsilon = (light.myCutOff - light.myOuterCutOff);
    float intensity = clamp((theta - light.myOuterCutOff) / epsilon, 0.0, 1.0);
    float distance = length(light.myPosition - fragPos);
    float attenuation = clamp(light.myIntensity - distance * distance / (light.myDistance * light.myDistance), 0.0, 1.0); attenuation *= attenuation;
    vec3 diffuse = light.myColor.xyz * objectColor * diff * intensity * attenuation;
    return diffuse * (1.0 - CalcSpotLightShadow(normal, fragPos, fragPosSpotLightSpace));
}

float Linstep(float low, float high, float v)
{
	return clamp((v-low)/(high-low), 0.0, 1.0);
}

float CalcDirectionalLightShadow(DirectionalLight light, vec3 normal, vec4 fragPosDirectionalLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosDirectionalLightSpace.xyz / fragPosDirectionalLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uDirectionalLightDepth, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	vec2 moments = texture2D(uDirectionalLightDepth, projCoords.xy).xy;
	
	float p = step(currentDepth, moments.x);
	float variance = max(moments.y - moments.x * moments.x, 0.00002);
	
	float d = currentDepth - moments.x;
	float pMax = Linstep(0.1, 1.0, variance / (variance + d*d));

	if(projCoords.z > 1.0) 
		return 1;
	
	return min(max(p, pMax), 1.0);

}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 objectColor, vec4 fragPosDirectionalLightSpace)
{
    vec3 lightDir = normalize(-light.myDirection);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = /*light.myColor.xyz **/ objectColor * diff;
    return (LightBuffer.myAmbientColor - clamp(-CalcDirectionalLightShadow(light, normal, fragPosDirectionalLightSpace) + diff, 0.0, 0.45) + diffuse) * objectColor.xyz;
}