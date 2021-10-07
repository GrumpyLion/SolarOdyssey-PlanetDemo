layout (std140, binding = 6) uniform WATER_BUFFER
{ 
    vec4 mySpecularColor;
    vec4 myWaterColor1;
    vec4 myWaterColor2;
    float myDepthMultiplier;
    float myAlphaMultiplier;
    float mySmoothness;
    float myWaveSpeed;
    float myWaveNormalScale;
    float myWaveStrength;
} WaterBuffer;