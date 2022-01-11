#pragma once

struct Mesh;
struct Material;
struct UniformBuffer;

struct Foliage
{
	enum class Type
	{
		Tree
	};
	Type myType = Type::Tree;
	Vec3 myPosition = Vec3(0.0f);
	Vec3 myDirection = Vec3(0.0f);
    Vec3 myScale = Vec3(1.0f);
    Mat4 myMatrix = Mat4(1.0);
};

struct FoliageBuffer
{
	static const int MaxInstances = 4069;
	Mat4 myTreeTransforms[MaxInstances];
	int myCurrentTreeInstances = 0;
};

class FoliageManager
{
public:
	FoliageManager();

	void AddFoliage(Foliage& foliage);
	void Render(float delta, RenderPass renderPass);

private:
	StaticArray<Foliage, FoliageBuffer::MaxInstances> myTrees;

    SPtr<Mesh> myTreeMesh;
    SPtr<Material> myTreeMaterial;
	SPtr<UniformBuffer> myTreeBuffer;

};