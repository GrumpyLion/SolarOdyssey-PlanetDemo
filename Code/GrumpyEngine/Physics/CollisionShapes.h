#pragma once

#include <PxPhysicsAPI.h>
struct MeshDesc;

struct BoxShape
{
    BoxShape(float halfWidth, float halfHeight, float halfDepth);
    BoxShape(const Vec3& halfExtends);

    inline operator physx::PxBoxGeometry()
    {
        return physx::PxBoxGeometry(physx::PxVec3(myHalfExtends.x, myHalfExtends.y, myHalfExtends.z));
    }

    Vec3 myHalfExtends{};
};

struct TriangleMeshShape
{
    TriangleMeshShape(const MeshDesc& desc);

    inline operator physx::PxTriangleMeshGeometry()
    {
        return myTriangleMeshGeometry;
    }

    physx::PxTriangleMeshGeometry myTriangleMeshGeometry{};
};

struct ConvexMeshShape
{
    ConvexMeshShape(const MeshDesc& desc);

    inline operator physx::PxConvexMeshGeometry()
    {
        return myConvexMesh;
    }

    physx::PxConvexMeshGeometry myConvexMesh{};
};