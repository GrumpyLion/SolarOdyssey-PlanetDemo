#include "pch.h"

#include "Texture.h"
#include "Sprite.h"

bool Sprite::Initialize(const std::string& location, bool centered /*= true*/)
{
    myCentered = centered;

    //myMaterial.myShader = ResourceManager::LoadShader("Sprite", "Assets/Shaders/Sprite.vs", "Assets/Shaders/Sprite.fs");
    //Texture* texture = ResourceManager::LoadTexture(location, GL_NEAREST, GL_REPEAT);
    //if (texture == nullptr)
    //    return false;

    //myWidth = texture->myWidth;
    //myHeight = texture->myHeight;
    //myMaterial.BindTexture("uTexture", texture);

    //MeshData desc{};
    //if (myCentered)
    //{
    //    desc.myVertices =
    //    {
    //        { -texture->myWidth / 2.0f, texture->myHeight / 2.0f, 0.0f },
    //        { -texture->myWidth / 2.0f, -texture->myHeight / 2.0f, 0.0f },
    //        { texture->myWidth / 2.0f, -texture->myHeight / 2.0f, 0.0f },

    //        { -texture->myWidth / 2.0f, texture->myHeight / 2.0f, 0.0f },
    //        { texture->myWidth / 2.0f, -texture->myHeight / 2.0f, 0.0f },
    //        { texture->myWidth / 2.0f, texture->myHeight / 2.0f, 0.0f }
    //    };
    //}
    //else
    //{
    //    desc.myVertices =
    //    {
    //        { 0.0f, 0.0f, 0.0f },
    //        { 0.0f, -texture->myHeight, 0.0f },
    //        { texture->myWidth, -texture->myHeight, 0.0f },

    //        { 0.0f, 0.0f, 0.0f },
    //        { texture->myWidth, -texture->myHeight, 0.0f },
    //        { texture->myWidth, 0.0f, 0.0f }
    //    };
    //}

    //desc.myTexcoords =
    //{
    //    { 0.0f, 1.0f },
    //    { 0.0f, 0.0f },
    //    { 1.0f, 0.0f },

    //    { 0.0f, 1.0f },
    //    { 1.0f, 0.0f },
    //    { 1.0f, 1.0f }
    //};

    //myMesh.Initialize(desc);

    return true;
}