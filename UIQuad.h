#pragma once
#include "Mesh.h"
#include <memory>

class GraphicsDevice;

std::shared_ptr<Mesh> CreateUIQuad(GraphicsDevice& device);

// u0..u1 の U 範囲で UV を作るクワッドを生成する（0〜1のUV）
std::shared_ptr<Mesh> CreateUIQuadWithUV(GraphicsDevice& device, float u0, float u1);

// v0..v1 の V 範囲で UV を作るクワッドを生成する（0〜1のUV）
// 縦方向に充填する UI のために追加
std::shared_ptr<Mesh> CreateUIQuadWithVRange(GraphicsDevice& device, float v0, float v1);