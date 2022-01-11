#pragma once

#include <map>

#include "Shader.h"
#include "Texture.h"

struct Material
{
    struct Sampler
    {
        uint myUnit{};
        SPtr<Texture> myTexture{};
    };

    static SPtr<Material> Create(const char* name, const char* vsLocation, const char* fsLocation);
    static SPtr<Material> Create(const char* vsLocation, const char* fsLocation);
    static SPtr<Material> Create(const char* name);

    static std::map<std::string, SPtr<Material>> ourMaterials;

    bool Bind(RenderPass currentPass = RenderPass::Geometry) const;
    
    void AddTexture(const char* name, SPtr<Texture> texture, uint unit = 0);
    bool AddShaderPass(const char* vsLocation, const char* fsLocation, RenderPass renderPass);

    std::array<SPtr<Shader>, RenderPass::Count> myShaders;
    std::unordered_map<std::string, Sampler> myTextures;
};