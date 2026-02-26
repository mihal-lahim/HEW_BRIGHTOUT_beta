#pragma once
#include "Mesh.h"
#include <memory>

class GraphicsDevice;

// 中心原点・Z=0 の板メッシュ（4頂点・6インデックス）を生成する
std::shared_ptr<Mesh> CreateBillboardQuad(GraphicsDevice& device);
