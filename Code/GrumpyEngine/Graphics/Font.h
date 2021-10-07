#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Renderer2D.h"

struct Font
{
    Font(const std::string& fontLocation,
        const std::string& textureLocation,
        const std::string& vertexShaderLocation,
        const std::string& fragmentShaderLocation);

    static Font Create(const std::string& fontLocation,
        const std::string& textureLocation,
        const std::string& vertexShaderLocation,
        const std::string& fragmentShaderLocation);

    static Font Create(const std::string& fontLocation, const std::string& textureLocation, bool centerText = false);

    void BuildMesh(const std::string& text, bool forceRebuild = false);

    //Ref<Material> myMaterial{}; // TODO
    //Ref<Mesh> myMesh{};
    float myTextureWidth = 0;
    float myTextureHeight = 0;
    float myHeight = 0;
    float myWidth = 0;
    bool myHasText = false;
    bool myCenterText = false;

    struct Char
    {
        float myCharID = 0;
        float myX = 0;
        float myY = 0;
        float myWidth = 0;
        float myHeight = 0;
        float myXOffset = 0;
        float myYOffset = 0;
        float myXAdvance = 0;
    };

    std::string myText;
    Char FindCharacter(int8 character);
    std::vector<Char> myChars;
};