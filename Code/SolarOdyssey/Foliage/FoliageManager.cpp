#include "pch.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

#include "FoliageManager.h"

FoliageManager::FoliageManager()
{
    myTreeMaterial = Material::Create("Tree");
    myTreeMesh = Mesh::Create("Data/SolarOdyssey/Models/Nature/Tree.glb");
}

void FoliageManager::AddFoliage(Foliage& foliage)
{
    foliage.myMatrix =
        Math::Translate(foliage.myPosition) *
        Math::GetRotationFromForward(foliage.myDirection, Vec3(0, 1, 0)) *
        Math::Rotate(Vec3(90 + Math::RandomFloat(-10.0f, 10.0f), Math::RandomFloat(-10.0f, 10.0f), Math::RandomFloat(-10.0f, 10.0f))) *
        Math::Scale(Vec3(1.0f) + Vec3(Math::RandomFloat(0.0f, .5f))) *
        myTreeMesh->myLocalMatrix;

    myTrees.Add(foliage);
}

void FoliageManager::Render(float delta, RenderPass renderPass)
{
    for (uint i = 0; i < myTrees.myCurrentCount; ++i) // TODO instancing
    {
        Foliage& tree = myTrees[i];
        myTreeMaterial->Bind(renderPass);
        myTreeMaterial->myShaders[renderPass]->BindMatrix4x4("uModelMatrix", tree.myMatrix);
        if (renderPass == RenderPass::MotionVectors)
            myTreeMaterial->myShaders[renderPass]->BindMatrix4x4("uPrevModelMatrix", tree.myMatrix);

        myTreeMesh->Render();
    }
}
