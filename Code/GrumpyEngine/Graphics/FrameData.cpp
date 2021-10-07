#include "pch.h"
#include "FrameData.h"

FrameData::FrameData()
{
    myInstances.reserve(4069);
}

void FrameData::AddInstance(SPtr<Mesh> mesh, SPtr<Material> material, const Mat4& matrix)
{
    Instance newInstance;
    newInstance.myMesh = mesh;
    newInstance.myMaterial = material;
    newInstance.myMatrix = matrix;
    myInstances.push_back(newInstance);
}

void FrameData::Clear()
{
    myInstances.clear();
}
