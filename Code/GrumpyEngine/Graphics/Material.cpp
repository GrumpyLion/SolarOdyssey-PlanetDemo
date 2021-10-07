#include "pch.h"

#include "Material.h"
#include "MaterialLibrary.h"

std::map<std::string, SPtr<Material>> Material::ourMaterials;

SPtr<Material> Material::Create(const char* name, const char* vsLocation, const char* fsLocation)
{
    auto mat = MakeShared<Material>();
    if (!mat->AddShaderPass(vsLocation, fsLocation, RenderPass::Geometry))
        mat->AddShaderPass("Data/GrumpyEngine/Shaders/Error.vs", "Data/GrumpyEngine/Shaders/Error.fs", RenderPass::Geometry);

    mat->AddShaderPass("Data/GrumpyEngine/Shaders/Depth.vs", "Data/GrumpyEngine/Shaders/Depth.fs", RenderPass::Depth); // TODO this needs to be defined in the json instead of here
    mat->AddShaderPass("Data/GrumpyEngine/Shaders/MotionVectors.vs", "Data/GrumpyEngine/Shaders/MotionVectors.fs", RenderPass::MotionVectors);

    ourMaterials[name] = mat;
    return mat;
}

SPtr<Material> Material::Create(const char* vsLocation, const char* fsLocation)
{
    auto mat = MakeShared<Material>();
    if (!mat->AddShaderPass(vsLocation, fsLocation, RenderPass::Geometry))
        mat->AddShaderPass("Data/GrumpyEngine/Shaders/Error.vs", "Data/GrumpyEngine/Shaders/Error.fs", RenderPass::Geometry);

    mat->AddShaderPass("Data/GrumpyEngine/Shaders/Depth.vs", "Data/GrumpyEngine/Shaders/Depth.fs", RenderPass::Depth); // TODO this needs to be defined in the json instead of here
    mat->AddShaderPass("Data/GrumpyEngine/Shaders/MotionVectors.vs", "Data/GrumpyEngine/Shaders/MotionVectors.fs", RenderPass::MotionVectors);
    return mat;
}

SPtr<Material> Material::Create(const char* name)
{
    return ourMaterials[name];
}

bool Material::Bind(RenderPass currentPass /*= RenderPass::Geometry*/) const
{
    if(myShaders[currentPass])
        myShaders[currentPass]->Bind();

    for (auto& sampler : myTextures)
    {
        myShaders[currentPass]->BindInt(sampler.first.c_str(), sampler.second.myUnit);
        sampler.second.myTexture->Bind(sampler.second.myUnit);
    }

    return true;
}

void Material::AddTexture(const char* name, SPtr<Texture> texture, uint unit /*= 0*/)
{
    myTextures[name].myUnit = unit;
    myTextures[name].myTexture = texture;
}

bool Material::AddShaderPass(const char* vsLocation, const char* fsLocation, RenderPass renderPass)
{
    myShaders[renderPass] = Shader::Create(vsLocation, fsLocation);
    return myShaders[renderPass] != nullptr;
}