#include "Mesh.h"

bool MeshBase::CreateBufferInternal(GraphicsDevice& device, const void* vertexes, const size_t vertexSize, const size_t vertexCount, const void* indexes, const size_t indexSize, const size_t indexCount)
{
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();

	// 頂点バッファの作成
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(vertexSize * vertexCount);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vertexData{};
	vertexData.pSysMem = vertexes;
	HRESULT hr = device.GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}
	// インデックスバッファの作成
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = static_cast<UINT>(indexSize * indexCount);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = indexes;
	hr = device.GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}
	m_vertexCount = static_cast<UINT>(vertexCount);
	m_indexCount = static_cast<UINT>(indexCount);
	return true;
}

void Mesh::Draw(GraphicsDevice& device) const
{
	UINT stride = sizeof(VertexAttribute);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = GetVertexBuffer();
	ID3D11Buffer* indexBuffer = GetIndexBuffer();
	if (!vertexBuffer || !indexBuffer)
	{
		return;
	}

	auto* context = device.GetDeviceContext();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(GetIndexCount(), 0, 0);
}

void SkinnedMesh::Draw(GraphicsDevice& device) const
{
	UINT stride = sizeof(VertexAttribute);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = GetVertexBuffer();
	ID3D11Buffer* indexBuffer = GetIndexBuffer();
	if (!vertexBuffer || !indexBuffer)
	{
		return;
	}

	auto* context = device.GetDeviceContext();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(GetIndexCount(), 0, 0);
}
