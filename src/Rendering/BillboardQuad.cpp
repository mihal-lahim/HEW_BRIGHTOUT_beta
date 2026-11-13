#include "BillboardQuad.h"
#include "GraphicsDevice.h"
#include <vector>

std::shared_ptr<Mesh> CreateBillboardQuad(GraphicsDevice& device)
{
    using VA = Mesh::VertexAttribute;

    std::vector<VA> vertices(4);

    // 左上
    vertices[0].position = { -0.5f,  0.5f, 0.0f };
    vertices[0].color    = {  1.0f,  1.0f, 1.0f, 1.0f };
    vertices[0].normal   = {  0.0f,  0.0f, -1.0f };
    vertices[0].uv       = {  0.0f,  0.0f };

    // 右上
    vertices[1].position = {  0.5f,  0.5f, 0.0f };
    vertices[1].color    = {  1.0f,  1.0f, 1.0f, 1.0f };
    vertices[1].normal   = {  0.0f,  0.0f, -1.0f };
    vertices[1].uv       = {  1.0f,  0.0f };

    // 左下
    vertices[2].position = { -0.5f, -0.5f, 0.0f };
    vertices[2].color    = {  1.0f,  1.0f, 1.0f, 1.0f };
    vertices[2].normal   = {  0.0f,  0.0f, -1.0f };
    vertices[2].uv       = {  0.0f,  1.0f };

    // 右下
    vertices[3].position = {  0.5f, -0.5f, 0.0f };
    vertices[3].color    = {  1.0f,  1.0f, 1.0f, 1.0f };
    vertices[3].normal   = {  0.0f,  0.0f, -1.0f };
    vertices[3].uv       = {  1.0f,  1.0f };

    // 三角形 0-1-2, 1-3-2
    std::vector<UINT> indices = { 0, 1, 2, 1, 3, 2 };

    auto mesh = std::make_shared<Mesh>();
    mesh->CreateBuffer(device, vertices, indices);

    return mesh;
}
