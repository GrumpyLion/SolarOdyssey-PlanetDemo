#pragma once

#include "tinygltf/tiny_gltf.h"

struct MeshDesc
{
    std::vector<Vec3> myPositions;
    std::vector<Vec3> myNormals;
    std::vector<Vec2> myUVs;
    std::vector<uint> myIndices;

    Vec3 myMin; // TODO
    Vec3 myMax;
};

struct Mesh
{    
    static SPtr<Mesh> Create(const char* fileLocation);
    static SPtr<Mesh> Create(const MeshDesc& desc);
    
    static std::map<std::string, SPtr<Mesh>> ourMeshes;

    ~Mesh();

    void Render() const;

    Mat4 myLocalMatrix = Mat4(1.0);
    Mat4 myPrevMatrix = Mat4(1.0);
    GLuint myVAO = 0;
    GLuint myBuffers[4];
    int myComponentType = 0;

    uint myIndicesCount = 0;
    uint myVerticesCount = 0;
};