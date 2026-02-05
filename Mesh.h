
#ifndef MESH_H
#define MESH_H

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include "GraphicsDevice.h"
#include "Resource.h"


// メッシュクラス(頂点タイプはテンプレート引数で指定)
class Mesh : public Resource
{
public:
	Mesh() = default;
	virtual ~Mesh() = default;

	// GPUバッファの作成
	template<typename VA>
	bool CreateBuffer(GraphicsDevice& device, const std::vector<VA>& vertexes, const std::vector<unsigned short>& indexes);

	// 一時的にMeshがやることになっている描画処理
	void Render(GraphicsDevice& device);

	// GPUバッファの取得
	ID3D11Buffer* const GetVertexBuffer() const 
	{ 
		return m_vertexBuffer.Get();
	}
	ID3D11Buffer* const GetIndexBuffer() const 
	{ 
		return m_indexBuffer.Get();
	}

protected:
	// 頂点ストライド
	UINT m_vertexStride = 0;

	// 頂点数とインデックス数
	UINT m_vertexCount = 0;
	UINT m_indexCount = 0;

private:
	// 頂点バッファとインデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = {};
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer = {};
};


// プリミティブメッシュクラス
class PrimitiveMesh : public Mesh
{
};

template<typename VS>
class CubeMesh : public PrimitiveMesh
{
public:
	using VertexAttribute = typename VS::VertexAttribute;
	template<typename VA>
	static std::vector<VA> CreateVertexes();
	static std::vector<unsigned short> CreateIndexes();
};

template<typename VS>
class PlaneMesh : public PrimitiveMesh
{
public:
	using VertexAttribute = typename VS::VertexAttribute;
	template<typename VA>
	static std::vector<VA> CreateVertexes();
	static std::vector<unsigned short> CreateIndexes();
};

template<typename VS>
class SphereMesh : public PrimitiveMesh
{
public:
	using VertexAttribute = typename VS::VertexAttribute;
	template<typename VA>
	static std::vector<VA> CreateVertexes();
	static std::vector<unsigned short> CreateIndexes();
};

template<typename VS>
class CapsuleMesh : public PrimitiveMesh
{
public:
	using VertexAttribute = typename VS::VertexAttribute;
	template<typename VA>
	static std::vector<VA> CreateVertexes();
	static std::vector<unsigned short> CreateIndexes();
};

template<typename VS>
class CylinderMesh : public PrimitiveMesh
{
public:
	using VertexAttribute = typename VS::VertexAttribute;
	template<typename VA>
	static std::vector<VA> CreateVertexes();
	static std::vector<unsigned short> CreateIndexes();
};

#include "Mesh.inl"

#endif
