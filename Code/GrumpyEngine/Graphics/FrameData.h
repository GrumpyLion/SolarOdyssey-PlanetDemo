#pragma once

#include "Material.h"
#include "Mesh.h"

struct FrameData
{
    struct Instance
    {
        SPtr<Mesh> myMesh;
        SPtr<Material> myMaterial;
        Mat4 myMatrix = Mat4(1.0f);
    };

    FrameData();

    void AddInstance(SPtr<Mesh> mesh, SPtr<Material> material, const Mat4& transform);
    void Clear();

    std::vector<Instance> myInstances; // TODO this needs to be split up. Meshes should have their own array to be referenced later and instanced instead of spawning many render calls
};
