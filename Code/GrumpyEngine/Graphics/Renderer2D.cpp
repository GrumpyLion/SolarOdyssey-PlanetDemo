#include "pch.h"

#include "Renderer2D.h"


void Renderer2D::Initialize()
{

}

void Renderer2D::Shutdown()
{

}

void Renderer2D::Submit(const RenderCommand& command)
{
    //locCommandBuffer.Push(command);
}

void Renderer2D::Execute()
{
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
    //for (const CommandBuffer::RenderCommand2D& command : ourCommandBuffer.myRenderCommands2D)
    //{
    //    if (command.myMaterial == nullptr)
    //    {
    //        continue;
    //    }
    //    Render2DCommand(command);
    //}
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    //Mat4 ortho = glm::ortho(0.0f, (float)Renderer::ourRenderTargetDimensions.x, 0.0f, (float)Renderer::ourRenderTargetDimensions.y);

    //std::sort(locPayloads.begin(), locPayloads.end(), [](UIPayload& left, UIPayload& right)
    //{
    //    return left.myPosition.z < right.myPosition.z;
    //});

    //for (uint i = 0; i < locPayloads.size(); ++i)
    //{
    //    const UIPayload& payload = locPayloads[i];

    //    Mat4 matrix = Mat4(1.0f);
    //    matrix *= glm::translate(Mat4(1.0f), payload.myPosition);

    //    matrix *= glm::translate(Mat4(1.0f), Vec3(0.5f * payload.mySize.x, 0.5f * payload.mySize.y, 0.0f));
    //    matrix *= glm::rotate(Mat4(1.0f), Math::ToRadians(payload.myRotation), Vec3(0.0f, 0.0f, 1.0f));
    //    matrix *= glm::translate(Mat4(1.0f), Vec3(-0.5f * payload.mySize.x, -0.5f * payload.mySize.y, 0.0f));

    //    matrix *= Math::Scale(Vec3(payload.mySize, 1.0f));

    //    //payload.myMaterial->BindMaterial();
    //    //payload.myMaterial->BindVector3f("uColor", payload.myColor);

    //    //Renderer::ourCommandBuffer.Push2D(payload.myMaterial, payload.myMesh, matrix, ortho);
    //}
}