#pragma once

enum class UniformBufferIndexes : uint
{
    CameraBuffer = 0,
    LightBuffer = 1,

    // Game specific, needs to be moved
    AtmosphereBuffer = 5,
    WaterBuffer = 6,
    FoliageBuffer = 7,

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

enum RenderPass
{
    Geometry = 0,
    Depth,
    Shadows,
    MotionVectors,
    Count
};