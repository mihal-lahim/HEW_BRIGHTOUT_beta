#pragma once
#include "Mesh.h"
#include <memory>

class GraphicsDevice;

std::shared_ptr<Mesh> CreateUIQuad(GraphicsDevice& device);

// u0..u1 の U 範囲で UV を作るクワッドを生成する（0〜1のUV）
std::shared_ptr<Mesh> CreateUIQuadWithUV(GraphicsDevice& device, float u0, float u1);