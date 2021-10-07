#include "pch.h"

#include "Font.h"

#include "Renderer2D.h"
#include "Renderer.h"

#include <sstream>
#include <fstream>

Font Font::Create(const std::string& fontLocation,
    const std::string& textureLocation,
    const std::string& vertexShaderLocation,
    const std::string& fragmentShaderLocation)
{
    return Font(fontLocation, textureLocation, vertexShaderLocation, fragmentShaderLocation);
}

Font::Font(const std::string& fontLocation, const std::string& textureLocation, const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation)
{
    myHeight = 0;

    std::fstream file(fontLocation);

    myChars.clear();
    std::string fontData((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    uint lastPos = 0;
    for (;;)
    {
        uint newPos = (uint)fontData.find('\n', lastPos + 1);
        std::string line = fontData.substr(lastPos, newPos - lastPos);
        lastPos = newPos;

        if (lastPos > fontData.size() || newPos > fontData.size())
            break;

        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> vstrings(begin, end);

        if (vstrings.size() == 7)
        {
            myTextureWidth = (float)atof(vstrings[3].substr(7).c_str());
            myTextureHeight = (float)atof(vstrings[4].substr(7).c_str());
        }

        if (vstrings.size() == 11)
        {
            Char c;
            c.myCharID = (float)atof(vstrings[1].substr(3).c_str());
            c.myX = (float)atof(vstrings[2].substr(2).c_str());
            c.myY = (float)atof(vstrings[3].substr(2).c_str());
            c.myWidth = (float)atof(vstrings[4].substr(6).c_str());
            c.myHeight = (float)atof(vstrings[5].substr(7).c_str());
            c.myXOffset = (float)atof(vstrings[6].substr(8).c_str());
            c.myYOffset = (float)atof(vstrings[7].substr(8).c_str());
            c.myXAdvance = (float)atof(vstrings[8].substr(9).c_str());

            myHeight = Math::Max(c.myHeight, myHeight);
            myWidth = Math::Max(c.myWidth, myWidth);

            myChars.push_back(c);
        }
    }

    //myMaterial = Material::Create(vertexShaderLocation, fragmentShaderLocation);
    //myMaterial->BindTexture("uTexture", Texture::Create(textureLocation));
}

Font Font::Create(const std::string& fontLocation, const std::string& textureLocation, bool centerText /*= false*/)
{
    Font font = Font::Create(fontLocation, textureLocation, "Data/GrumpyEngine/Shaders/Font.vs", "Data/GrumpyEngine/Shaders/Font.fs");
    font.myCenterText = centerText;
    return font;
}

Font::Char Font::FindCharacter(int8 character)
{
    for (uint i = 0; i < myChars.size(); ++i)
    {
        if (myChars[i].myCharID == character)
        {
            return myChars[i];
        }
    }

    return Char();
}

void Font::BuildMesh(const std::string& text, bool forceRebuild /*= false*/)
{
    if (text.empty())
    {
        myHasText = false;
        return;
    }

    if (myText.compare(text) == 0 && !forceRebuild)
    {
        return;
    }
    myText = text;
    myHasText = true;

    float cursorXPos = 0;
    float cursorYPos = 0;
    float textWidth = 0.0f;
    MeshDesc desc{};

    for (int i = 0; i < text.length(); ++i)
    {
        int8 character = text.at(i);
        if (character == '\n')
        {
            cursorXPos = 0.0f;
            cursorYPos += myHeight;
        }

        Char c = FindCharacter(character);

        desc.myPositions.push_back({ (cursorXPos + c.myXOffset), (cursorYPos + c.myYOffset), 0.0f });
        desc.myPositions.push_back({ (cursorXPos + c.myXOffset), (cursorYPos + c.myYOffset + c.myHeight), 0.0f });
        desc.myPositions.push_back({ (cursorXPos + c.myXOffset + c.myWidth), (cursorYPos + c.myYOffset + c.myHeight), 0.0f });

        desc.myPositions.push_back({ (cursorXPos + c.myXOffset), (cursorYPos + c.myYOffset), 0.0f });
        desc.myPositions.push_back({ (cursorXPos + c.myXOffset + c.myWidth), (cursorYPos + c.myYOffset + c.myHeight), 0.0f });
        desc.myPositions.push_back({ (cursorXPos + c.myXOffset + c.myWidth), (cursorYPos + c.myYOffset), 0.0f });

        float xt = c.myX;
        float yt = myTextureHeight - c.myY;

        desc.myUVs.push_back({ xt / myTextureWidth, yt / myTextureHeight });
        desc.myUVs.push_back({ xt / myTextureWidth, (yt - c.myHeight) / myTextureHeight });
        desc.myUVs.push_back({ (xt + c.myWidth) / myTextureWidth, (yt - c.myHeight) / myTextureHeight });

        desc.myUVs.push_back({ (xt / myTextureWidth), yt / myTextureHeight });
        desc.myUVs.push_back({ (xt + c.myWidth) / myTextureWidth, (yt - c.myHeight) / myTextureHeight });
        desc.myUVs.push_back({ (xt + c.myWidth) / myTextureWidth, yt / myTextureHeight });

        textWidth += c.myWidth;
        cursorXPos += c.myXAdvance;
    }

    if (myCenterText)
    {
        for (auto& vertex : desc.myPositions)
        {
            vertex.x -= textWidth / 2.0f;
            vertex.y -= (cursorYPos + myHeight) / 2.0f;
        }
    }

    //myMesh = Mesh::Create(desc); // TODO
}