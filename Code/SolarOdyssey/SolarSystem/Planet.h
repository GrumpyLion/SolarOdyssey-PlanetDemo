#pragma once

#include "Physics/Rigidbody.h"
#include "Common/SimplexNoise.h"
#include "Graphics/Mesh.h"

struct Material;

struct Planet
{
    Planet();

    void Render(float delta);

#if !GE_FINAL
    bool myRenderDebug = false;
    void DebugRender();
#endif

    float GetNoiseFromUnitPosition(const Vec3& unitPos) const;

    struct TerrainFace
    {
        TerrainFace(int resolution, Vec3 localUp)
            : myResolution(resolution)
            , myLocalUp(localUp)
        {
            myAxisA = Vec3(myLocalUp.y, myLocalUp.z, myLocalUp.x);
            myAxisB = glm::cross(myLocalUp, myAxisA);
        }

        TerrainFace()
            : myResolution(0)
            , myLocalUp(Vec3(0))
        {
            myAxisA = Vec3(myLocalUp.y, myLocalUp.z, myLocalUp.x);
            myAxisB = glm::cross(myLocalUp, myAxisA);
        }

        void ConstructMesh(const Planet& planet);

        MeshDesc myMeshDescription;
        SPtr<Mesh> myMesh;
        int myResolution;
        Vec3 myLocalUp;
        Vec3 myAxisA;
        Vec3 myAxisB;
    };
    
    void Generate();

    struct NoiseParams
    {
        int myOctaveCount = 5;
        float myNoiseStrength = 1.0f;
        float myNoisePosScale = 1.0f;
        float myNoiseMax = 1.0f;
        float myNoiseMin = 0.0f;
        Vec3 myNoisePosition{};
    };

    NoiseParams myContinentParams;
    //NoiseParams myMountainMaskParams;
    NoiseParams myElevationParams;

    int myResolution = 50;
    float myPlanetSize = 50.0f;
    float myWaterLevel = 190.0f;

    float myTextureScale = 1.0f;
    float myBlendSharpness = 10.0f;
    SimplexNoise myNoise;
    TerrainFace myTerrainFaces[6];
    TerrainFace myTerrainFacesCollision[6];

    SPtr<RigidbodyStatic> myRigidBody;
    SPtr<Material> myMaterial;
};