
#ifndef MESH_H
#define MESH_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include "GraphicsDevice.h"
#include "Resource.h"

// 頂点構造体
struct Vertex3d
{
	DirectX::XMFLOAT3 position; // 頂点座標
	DirectX::XMFLOAT4 color;    // カラー
	DirectX::XMFLOAT3 normal;   // 法線ベクトル
	DirectX::XMFLOAT2 texcoord; // テクスチャ座標
};

class Mesh : public Resource
{
public:
	Mesh() = default;
	virtual ~Mesh() = default;

	// GPUバッファの作成
	void Initialize(GraphicsDevice& device) override;

	// 頂点データとインデックスデータの追加
	void AddVertex(const Vertex3d& vertex)
	{
		m_vertices.push_back(vertex);
	}
	void AddIndex(unsigned short index)
	{
		m_indices.push_back(index);
	}

	// 一時的にMeshがやることになっている描画処理
	void Render(GraphicsDevice& device);

	// GPUバッファの取得
	ID3D11Buffer* const GetVertexBuffer() const { return m_vertexBuffer.Get(); }
	ID3D11Buffer* const GetIndexBuffer() const { return m_indexBuffer.Get(); }

	// 頂点データとインデックスデータの取得
	std::vector<Vertex3d>& GetVertices() { return m_vertices; }
	std::vector<unsigned short>& GetIndices() { return m_indices; }

private:
	// 頂点バッファとインデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = {};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = {};

	// 頂点データとインデックスデータの配列
	std::vector<Vertex3d> m_vertices = {};
	std::vector<unsigned short> m_indices = {};
};


class CubeMesh : public Mesh
{
public:
	CubeMesh();
};

class PlaneMesh : public Mesh
{
public:
	PlaneMesh();
};

class SphereMesh : public Mesh
{
public:
	SphereMesh();
};

class CapsuleMesh : public Mesh
{
public:
	CapsuleMesh();
};

class CylinderMesh : public Mesh
{
public:
	CylinderMesh();
};


#endif
