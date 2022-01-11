#include "pch.h"

#include "Material.h"
#include "MaterialLibrary.h"

#include <fstream>
#include <nlohmann/json.hpp>

bool MaterialLibrary::LoadFromJSON(const char* location)
{
    std::ifstream file(location);
    nlohmann::json json;
    if (file.is_open())
        json = nlohmann::json::parse(file);
    else
        return false;

    for (auto materialNode : json)
    {
        SPtr<Material> material{};
        std::string name = materialNode["name"];
        std::string vsLocation{};
        std::string fsLocation{};
        std::string gsLocation{};

        if (!materialNode["vs"].is_null())
            vsLocation = materialNode["vs"].get<std::string>();

        if (!materialNode["fs"].is_null())
            fsLocation = materialNode["fs"].get<std::string>();

        if (!materialNode["gs"].is_null())
            gsLocation = materialNode["gs"].get<std::string>();

        material = Material::Create(name.c_str(), vsLocation.c_str(), fsLocation.c_str());

        if (!materialNode["shaders"].is_null())
        {
            for (auto shaderNode : materialNode["shaders"])
            {
                std::string type = shaderNode["type"].get<std::string>();

                if (!shaderNode["vs"].is_null())
                    vsLocation = shaderNode["vs"].get<std::string>();

                if (!shaderNode["fs"].is_null())
                    fsLocation = shaderNode["fs"].get<std::string>();

                RenderPass pass = RenderPass::Geometry;
                if (type.compare("depth") == 0)
                    pass = RenderPass::Depth;
                else if (type.compare("motion") == 0)
                    pass = RenderPass::MotionVectors;
                else if (type.compare("shadows") == 0)
                    pass = RenderPass::Shadows;

                material->AddShaderPass(vsLocation.c_str(), fsLocation.c_str(), pass);
            }
        }

        if (!materialNode["textures"].is_null())
        {
            int index = 0;
            for (auto textureNode : materialNode["textures"])
            {
                std::string name = textureNode["name"].get<std::string>();
                std::string location{};
                std::string filter{};
                std::string wrap{};

                if (!textureNode["location"].is_null())
                    location = textureNode["location"].get<std::string>();

                if (!textureNode["filter"].is_null())
                    filter = textureNode["filter"].get<std::string>();

                if (!textureNode["wrap"].is_null())
                    wrap = textureNode["wrap"].get<std::string>();

                GLenum filterVal = GL_NEAREST;
                if (!filter.empty() && filter.compare("linear") == 0)
                    filterVal = GL_LINEAR;

                GLenum wrapVal = GL_CLAMP_TO_BORDER;
                if (!wrap.empty() && wrap.compare("repeat") == 0)
                    wrapVal = GL_REPEAT;

                if (!wrap.empty() && wrap.compare("mirror") == 0)
                    wrapVal = GL_MIRRORED_REPEAT;

                material->AddTexture(name.c_str(), Texture::Create(location, filterVal, wrapVal), index++);
            }
        }
    }

    return true;
}