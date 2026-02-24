#include "UIQuad.h"
#include "GraphicsDevice.h"
#include <vector>

std::shared_ptr<Mesh> CreateUIQuad(GraphicsDevice& device)
{
    using VA = Mesh::VertexAttribute;

    std::vector<VA> vertices(4);

    vertices[0].position = { 0,0,0 };
    vertices[0].uv = { 0,0 };

    vertices[1].position = { 1,0,0 };
    vertices[1].uv = { 1,0 };

    vertices[2].position = { 1,1,0 };
    vertices[2].uv = { 1,1 };

    vertices[3].position = { 0,1,0 };
    vertices[3].uv = { 0,1 };

    std::vector<UINT> indices = { 0,1,2, 0,2,3 };

    auto mesh = std::make_shared<Mesh>();
    mesh->CreateBuffer(device, vertices, indices);

    return mesh;
}

std::shared_ptr<Mesh> CreateUIQuadWithUV(GraphicsDevice& device, float u0, float u1)
{
    using VA = Mesh::VertexAttribute;

    std::vector<VA> vertices(4);

    // 位置は 0..1 の正方形。UV の U は u0..u1 に変更する。
    vertices[0].position = { 0,0,0 };
    vertices[0].uv = { u0,0 };

    vertices[1].position = { 1,0,0 };
    vertices[1].uv = { u1,0 };

    vertices[2].position = { 1,1,0 };
    vertices[2].uv = { u1,1 };

    vertices[3].position = { 0,1,0 };
    vertices[3].uv = { u0,1 };

    std::vector<UINT> indices = { 0,1,2, 0,2,3 };

    auto mesh = std::make_shared<Mesh>();
    mesh->CreateBuffer(device, vertices, indices);

    return mesh;
}