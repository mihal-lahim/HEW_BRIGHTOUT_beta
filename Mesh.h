#ifndef MESH_H
#define MESH_H


#include "GraphicsDevice.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class MeshBase
{
public:
	UINT GetVertexCount() const
	{
		return m_vertexCount;
	}
	UINT GetIndexCount() const
	{
		return m_indexCount;
	}
	ID3D11Buffer* const GetVertexBuffer() const
	{
		return m_vertexBuffer.Get();
	}
	ID3D11Buffer* const GetIndexBuffer() const
	{
		return m_indexBuffer.Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = nullptr;
	UINT m_vertexCount = 0;
	UINT m_indexCount = 0;
protected:

	// バッファの作成の共通処理
	bool CreateBufferInternal(GraphicsDevice& device, const void* vertexes, const size_t vertexSize, const size_t vertexCount,
		const void* indexes, const size_t indexSize, const size_t indexCount);
};

// 通常メッシュクラス
class Mesh : public MeshBase
{
public:
	// 頂点構造体
	struct VertexAttribute
	{
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT4 color = {};
		DirectX::XMFLOAT3 normal = {};
		DirectX::XMFLOAT2 uv = {};
	};

	bool CreateBuffer(GraphicsDevice& device, const std::vector<VertexAttribute>& vertexes, const std::vector<UINT>& indexes)
	{
		return CreateBufferInternal(device, vertexes.data(), sizeof(VertexAttribute), vertexes.size(),
			indexes.data(), sizeof(unsigned int), indexes.size());
	}

	// 描画処理
	void Draw(GraphicsDevice& device) const;
};

// スキンドメッシュクラス
class SkinnedMesh : public MeshBase
{
public:
	// 頂点構造体
	struct VertexAttribute
	{
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT4 color = {};
		DirectX::XMFLOAT3 normal = {};
		DirectX::XMFLOAT2 uv = {};
		UINT bone[4] = {};
		float weight[4] = {};
	};

	bool CreateBuffer(GraphicsDevice& device, const std::vector<VertexAttribute>& vertexes, const std::vector<UINT>& indexes)
	{
		return CreateBufferInternal(device, vertexes.data(), sizeof(VertexAttribute), vertexes.size(),
			indexes.data(), sizeof(unsigned int), indexes.size());
	}

	// 描画処理
	void Draw(GraphicsDevice& device) const;
};

#endif
