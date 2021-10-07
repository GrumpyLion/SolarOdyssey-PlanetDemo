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
	mat4 myPrevViewMatrix;
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

float LinearDepth(float depthSample)
{
    const float near = CameraBuffer.myNearFarFOV.x;
    const float far = CameraBuffer.myNearFarFOV.y;
    return near * far / (far + depthSample * (near - far));
}