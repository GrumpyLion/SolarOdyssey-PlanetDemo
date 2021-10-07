#pragma once

#include "Mesh.h"
#include "Material.h"

struct Sprite
{
    bool Initialize(const std::string& location, bool centered = true);

    //Ref<Material> myMaterial{}; // TODO
    //Ref<Mesh> myMesh{};
    Vec3 myColor = Vec3(1.0);
    int myWidth = 0;
    int myHeight = 0;
    bool myCentered = true;
};