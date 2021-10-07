#pragma once

enum class UniformBufferIndexes : uint
{
    CameraBuffer = 0,
    LightBuffer = 1,

    // Game specific
    AtmosphereBuffer = 5,
    WaterBuffer = 6,

    PointLightShadowBuffer = 10,
    SpotLightShadowBuffer = 11,
    DirectionalLightShadowBuffer = 12
};

enum class TextureIndexes : uint
{
    Color = 0,
    Normals,
    PointLightDepth = 10,
    SpotLightDepth = 11,
    DirectionalLightDepth = 12
};