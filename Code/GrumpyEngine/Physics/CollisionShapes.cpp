#include "pch.h"

#include "Graphics/Mesh.h"
#include "CollisionShapes.h"
#include "Physics/PhysicsManager.h"

using namespace physx;

BoxShape::BoxShape(float halfWidth, float halfHeight, float halfDepth)
    : myHalfExtends(halfWidth, halfHeight, halfDepth)
{

}

BoxShape::BoxShape(const Vec3& halfExtends)
    : myHalfExtends(halfExtends)
{

}

TriangleMeshShape::TriangleMeshShape(const MeshDesc& desc)
{
    PxTriangleMeshDesc meshDesc{};
    meshDesc.points.count = (uint)desc.myPositions.size();
    meshDesc.points.stride = sizeof(Vec3);
    meshDesc.points.data = desc.myPositions.data();

    meshDesc.triangles.count = (uint)desc.myIndices.size() / 3;
    meshDesc.triangles.stride = 3 * sizeof(uint);
    meshDesc.triangles.data = desc.myIndices.data();

    PxDefaultMemoryOutputStream writeBuffer{};
    PxTriangleMeshCookingResult::Enum result{};
    bool status = PhysicsManager::ourCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

    GE_ASSERT(status);

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    auto collisionMesh = PhysicsManager::ourPhysics->createTriangleMesh(readBuffer);
    myTriangleMeshGeometry.triangleMesh = collisionMesh;
}

ConvexMeshShape::ConvexMeshShape(const MeshDesc& desc)
{
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = (PxU32)desc.myPositions.size();
    convexDesc.points.stride = sizeof(Vec3);
    convexDesc.points.data = desc.myPositions.data();
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxDefaultMemoryOutputStream buf;
    PxConvexMeshCookingResult::Enum result;
    if (!PhysicsManager::ourCooking->cookConvexMesh(convexDesc, buf, &result))
        return;
    PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
    myConvexMesh = PxConvexMeshGeometry(PhysicsManager::ourPhysics->createConvexMesh(input));
}
