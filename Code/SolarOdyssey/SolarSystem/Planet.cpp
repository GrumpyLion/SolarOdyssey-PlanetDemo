#include "pch.h"

#include "Physics/PhysicsManager.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Renderer.h"
#include "Physics/CollisionShapes.h"
#include "Graphics/Renderer.h"

#include "SolarOdyssey.h"
#include "Rendering/SolarOdysseyRenderer.h"

#include "Planet.h"

#if !GE_FINAL
#include <imgui.h>
#include "Editor/TopMenuBar.h"
#endif

using namespace physx;

Planet::Planet()
{
    myContinentParams.myOctaveCount = 7;
    myContinentParams.myNoiseStrength = 2.5f;
    myContinentParams.myNoisePosScale = 0.4f;
    myContinentParams.myNoiseMin = -5.0f;
    myContinentParams.myNoiseMax = 0.5f;
    
    myElevationParams.myOctaveCount = 7;
    myElevationParams.myNoiseStrength = 2.0f;
    myElevationParams.myNoisePosScale = 1;

    myMaterial = Material::Create("Planet");
    Generate();

    for (uint i = 0; i < 15; ++i)
    {
        Vec3 unitPos = Vec3(0.0f);
        
        unitPos += Vec3(Math::RandomFloat(-1.0f, 1.0f), Math::RandomFloat(-1.0f, 1.0f), Math::RandomFloat(-1.0f, 1.0f));
        unitPos = Math::Normalize(unitPos);
        Vec3 lastTreePos = Vec3(0.0f);
        for (uint j = 0; j < 50; ++j)
        {
            unitPos += Vec3(Math::RandomFloat(-.25f, .25f), Math::RandomFloat(-.25f, .25f), Math::RandomFloat(-.25f, .25f));
            unitPos = Math::Normalize(unitPos);

            float height = GetNoiseFromUnitPosition(unitPos);
            if (height >= myPlanetSize - 0.25f)
            {
                Foliage tree{};
                tree.myPosition = unitPos * (height - .5f);
                tree.myDirection = Math::Normalize(unitPos);
                if (Math::Distance(tree.myPosition, lastTreePos) > 10.0f)
                {
                    SolarOdyssey::ourRenderer->myFoliageManager.AddFoliage(tree);
                    lastTreePos = tree.myPosition;
                }
            }
        }
    }

#if !GE_FINAL
    TopMenuBar::ourMenus["Game"].myItems["Planet Editor"].myOnKlicked.Subscribe(this, [&](bool active) { myRenderDebug = active; });
#endif
}

void Planet::Render(float delta)
{
#if !GE_FINAL
    if(myRenderDebug)
        DebugRender();
#endif

    myMaterial->Bind();

    myMaterial->myShaders[RenderPass::Geometry]->BindFloat("uTextureScale", myTextureScale);
    myMaterial->myShaders[RenderPass::Geometry]->BindFloat("uBlendSharpness", myBlendSharpness);

    PxTransform transform = myRigidBody->myRigidbody->getGlobalPose();
    Vec3 position = Vec3(transform.p.x, transform.p.y, transform.p.z);
    Quat q = Quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
    for (int i = 0; i < 6; i++)
    {
        Renderer::ourFrameData.AddInstance(myTerrainFaces[i].myMesh, myMaterial, Math::Translate(position) * Math::Rotate(q));
    }
}

#if !GE_FINAL
void Planet::DebugRender()
{
    if (!ImGui::Begin("Planet Editor", &myRenderDebug))
    {
        ImGui::End();
        return;
    }

    ImGui::DragFloat("Texture Scale", &myTextureScale, 0.1f);
    ImGui::DragFloat("Blend Sharpness", &myBlendSharpness, 0.1f);

    ImGui::DragInt("Resolution", &myResolution);

    ImGui::DragFloat("Planet Size", &myPlanetSize, 0.1f);
    ImGui::DragFloat("Water Level", &myWaterLevel, 0.1f);

    ImGui::Separator();
    ImGui::DragInt("Continent Octave Count", &myContinentParams.myOctaveCount);
    ImGui::DragFloat("Continent Noise Strength", &myContinentParams.myNoiseStrength, 0.01f);
    ImGui::DragFloat("Continent Noise Pos Scale", &myContinentParams.myNoisePosScale, 0.01f);
    ImGui::DragFloat("Continent Noise Max", &myContinentParams.myNoiseMax, 0.1f);
    ImGui::DragFloat("Continent Noise Min", &myContinentParams.myNoiseMin, 0.1f);
    ImGui::DragFloat3("Continent Noise Pos", (float*)&myContinentParams.myNoisePosition, 0.1f);

    ImGui::Separator();
    ImGui::DragInt("Elevation Octave Count", &myElevationParams.myOctaveCount);
    ImGui::DragFloat("Elevation Noise Strength", &myElevationParams.myNoiseStrength, 0.01f);
    ImGui::DragFloat("Elevation Noise Pos Scale", &myElevationParams.myNoisePosScale, 0.01f);
    ImGui::DragFloat3("Elevation Noise Pos", (float*)&myElevationParams.myNoisePosition, 0.1f);

    if (ImGui::Button("Regenerate Surface"))
        Generate();

    ImGui::End();
}
#endif

float Planet::GetNoiseFromUnitPosition(const Vec3& unitPos) const
{
    Vec3 continentNoisePos = (unitPos + myContinentParams.myNoisePosition) * myContinentParams.myNoisePosScale;
    float continentShape = myNoise.Fractal(myContinentParams.myOctaveCount, continentNoisePos.x, continentNoisePos.y, continentNoisePos.z) * myContinentParams.myNoiseStrength;
    continentShape = Math::Clamp(continentShape, myContinentParams.myNoiseMin, myContinentParams.myNoiseMax);

    Vec3 elevationNoisePos = (unitPos + myElevationParams.myNoisePosition) * myElevationParams.myNoisePosScale;
    float elevation = myNoise.Fractal(myElevationParams.myOctaveCount, elevationNoisePos.x, elevationNoisePos.y, elevationNoisePos.z) * myElevationParams.myNoiseStrength;

    return myPlanetSize * (1.0f + continentShape * 0.01f + (elevation * 0.01f * continentShape));
}

void Planet::Generate()
{
    if (myRigidBody)
        myRigidBody.reset();

    Vec3 directions[6] = { Vec3(0, 1, 0), Vec3(0, -1, 0) , Vec3(1, 0, 0), Vec3(-1, 0, 0) , Vec3(0, 0, 1), Vec3(0, 0, -1) };
    for (int i = 0; i < 6; i++)
    {
        myTerrainFaces[i] = TerrainFace(myResolution, directions[i]);
        myTerrainFaces[i].ConstructMesh(*this);

        myTerrainFacesCollision[i] = TerrainFace(20, directions[i]);
        myTerrainFacesCollision[i].ConstructMesh(*this);

        if (!myRigidBody)
        {
            myRigidBody = MakeShared<RigidbodyStatic>(TriangleMeshShape(myTerrainFacesCollision[i].myMeshDescription)/*TriangleMeshShape(myTerrainFacesCollision[i].myMeshDescription)*/);
        }
        else
        {
            physx::PxShape* shape = PhysicsManager::ourPhysics->createShape(TriangleMeshShape(myTerrainFacesCollision[i].myMeshDescription), *PhysicsManager::ourPhysics->createMaterial(0.5f, 0.5f, 0.6f)); // TODO this probably leaks
            myRigidBody->myRigidbody->attachShape(*shape);
        }
    }
}

void Planet::TerrainFace::ConstructMesh(const Planet& planet)
{
    myResolution = planet.myResolution;
    myMeshDescription.myPositions.resize(myResolution * myResolution);
    myMeshDescription.myNormals.resize(myResolution * myResolution);
    myMeshDescription.myIndices.resize((myResolution - 1) * (myResolution - 1) * 6);

    int triIndex = 0;
    for (int y = 0; y < myResolution; ++y)
    {
        for (int x = 0; x < myResolution; ++x)
        {
            int i = x + y * myResolution;
            Vec2 percent = Vec2(x, y) / Vec2(myResolution - 1.0f);
            Vec3 pointOnUnitCube = myLocalUp + (percent.x - 0.5f) * 2.0f * myAxisA + (percent.y - 0.5f) * 2.0f * myAxisB;
            Vec3 pointOnUnitSphere = Math::Normalize(pointOnUnitCube);

            const Vec3 pos = pointOnUnitSphere * planet.GetNoiseFromUnitPosition(pointOnUnitSphere);
            myMeshDescription.myPositions[i] = pos;
            myMeshDescription.myNormals[i] = Math::Normalize(myMeshDescription.myPositions[i]);

            if (x != myResolution - 1 && y != myResolution - 1)
            {
                myMeshDescription.myIndices[triIndex] = i;
                myMeshDescription.myIndices[triIndex + 1] = i + myResolution + 1;
                myMeshDescription.myIndices[triIndex + 2] = i + myResolution;

                myMeshDescription.myIndices[triIndex + 3] = i;
                myMeshDescription.myIndices[triIndex + 4] = i + 1;
                myMeshDescription.myIndices[triIndex + 5] = i + myResolution + 1;
                triIndex += 6;
            }
        }
    }

    myMesh = Mesh::Create(myMeshDescription);
}