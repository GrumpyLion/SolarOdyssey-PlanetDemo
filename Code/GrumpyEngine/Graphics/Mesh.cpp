#include "pch.h"

#include "Mesh.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14 
#include "tinygltf/tiny_gltf.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::map<std::string, SPtr<Mesh>> Mesh::ourMeshes;

SPtr<Mesh> Mesh::Create(const char* filelocation)
{
    if (ourMeshes.find(filelocation) != ourMeshes.end())
        return ourMeshes[filelocation];

    auto meshRef = MakeShared<Mesh>();

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filelocation);

    if (!warn.empty())
        GE_PRINT("Warn: %s\n", warn.c_str());

    if (!err.empty())
        GE_PRINT("Err: %s\n", err.c_str());

    if (!ret)
    {
        GE_PRINT("Failed to parse glTF\n");
        return nullptr;
    }

    //for (int i = 0; i < model.meshes.size(); ++i)
    {
        tinygltf::Mesh mesh = model.meshes[0]; // only supports one mesh for now.. TODO add multimeshes
        tinygltf::Node node = model.nodes[0];

        Vec3 translation{};
        Quat rotation{};
        Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);;

        if (node.translation.size() == 3)
            translation = Math::MakeVec3(node.translation.data());

        if (node.rotation.size() == 4)
            rotation = Math::MakeQuat(node.rotation.data());

        if (node.scale.size() == 3)
            scale = Math::MakeVec3(node.scale.data());

        meshRef->myLocalMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale);

        glGenVertexArrays(1, &meshRef->myVAO);
        glBindVertexArray(meshRef->myVAO);

        std::map<int, GLuint> vbos;
        for (int i = 0; i < model.bufferViews.size(); ++i)
        {
            const tinygltf::BufferView& bufferView = model.bufferViews[i];
            if (bufferView.target == 0)
            {
                GE_PRINT("WARN: bufferView.target is zero");
                continue;
            }

            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
            GLuint vbo;
            glGenBuffers(1, &vbo);
            vbos[i] = vbo;
            glBindBuffer(bufferView.target, vbo);
            glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }

        for (size_t i = 0; i < mesh.primitives.size(); ++i)
        {
            tinygltf::Primitive primitive = mesh.primitives[i];
            tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

            meshRef->myIndicesCount = (uint)indexAccessor.count;
            meshRef->myComponentType = indexAccessor.componentType;

            for (auto& attrib : primitive.attributes)
            {
                tinygltf::Accessor accessor = model.accessors[attrib.second];
                int byteStride =
                    accessor.ByteStride(model.bufferViews[accessor.bufferView]);
                glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

                int size = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR)
                    size = accessor.type;

                int vaa = -1;
                if (attrib.first.compare("POSITION") == 0) vaa = 0;
                if (attrib.first.compare("NORMAL") == 0) vaa = 1;
                if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
                if (vaa > -1)
                {
                    glEnableVertexAttribArray(vaa);
                    glVertexAttribPointer(vaa, size, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, byteStride, BUFFER_OFFSET(accessor.byteOffset));
                }
                else
                    GE_PRINT("vaa missing: %s", attrib.first.c_str());
            }
        }
    }

    glBindVertexArray(0);

    ourMeshes[filelocation] = meshRef;
    return meshRef;
}

SPtr<Mesh> Mesh::Create(const MeshDesc& desc)
{
    auto meshRef = MakeShared<Mesh>();

    glGenVertexArrays(1, &meshRef->myVAO);

    glGenBuffers(4, meshRef->myBuffers);

    meshRef->myVerticesCount = (uint)desc.myPositions.size();
    meshRef->myIndicesCount = (uint)desc.myIndices.size();
    if (meshRef->myIndicesCount > 0)
        meshRef->myComponentType = GL_UNSIGNED_INT;

    glBindVertexArray(meshRef->myVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshRef->myBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, desc.myPositions.size() * sizeof(Vec3), &desc.myPositions[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    if (desc.myNormals.size() > 0)
    {
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, meshRef->myBuffers[1]);
        glBufferData(GL_ARRAY_BUFFER, desc.myNormals.size() * sizeof(Vec3), &desc.myNormals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(
            1,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
    }

    if (desc.myUVs.size() > 0)
    {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, meshRef->myBuffers[2]);
        glBufferData(GL_ARRAY_BUFFER, desc.myUVs.size() * sizeof(Vec2), &desc.myUVs[0], GL_STATIC_DRAW);
        glVertexAttribPointer(
            2,                  // attribute
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
    }

    if (desc.myIndices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshRef->myBuffers[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.myIndices.size() * sizeof(int), &desc.myIndices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);

    return meshRef;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &myVAO);
    glDeleteBuffers(4, myBuffers);
}

void Mesh::Render() const
{
    glBindVertexArray(myVAO);
    if (myIndicesCount > 0)
        glDrawElements(GL_TRIANGLES, myIndicesCount, myComponentType, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, myVerticesCount);
}
