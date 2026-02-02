#include "Mesh.h"
#include <cmath>
#include <stdexcept>

namespace
{
	constexpr float kHalfSize = 0.5f;
	constexpr int kDefaultSlices = 16;
	constexpr int kDefaultStacks = 16;
	constexpr int kDefaultHemisphereStacks = 8;

	constexpr float kPi = 3.14159265358979323846f;
}

void Mesh::CreateBuffers(GraphicsDevice* device)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(Vertex3d) * m_vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	// 頂点バッファへ流し込むデータの設定
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = m_vertices.data();

	HRESULT result_v = device->GetDevice()->CreateBuffer(&bd, &sd, m_vertexBuffer.GetAddressOf());
	if (FAILED(result_v)) {
		throw std::runtime_error("頂点バッファの作成に失敗しました");
	}
	// インデックスバッファ生成
	bd.ByteWidth = static_cast<UINT>(sizeof(unsigned short) * m_indices.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// インデックスバッファへ流し込むデータの設定
	sd.pSysMem = m_indices.data();
	HRESULT result_i = device->GetDevice()->CreateBuffer(&bd, &sd, m_indexBuffer.GetAddressOf());
	if (FAILED(result_i)) {
		throw std::runtime_error("インデックスバッファの作成に失敗しました");
	}
}

void Mesh::Render(GraphicsDevice* device)
{
	ID3D11DeviceContext* context = device->GetDeviceContext();
	// 頂点バッファの設定
	UINT stride = sizeof(Vertex3d);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// インデックスバッファの設定
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	// プリミティブトポロジーの設定（三角形リスト）
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画
	context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
}

CubeMesh::CubeMesh()
{
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float h = kHalfSize;

	auto addFace = [this, &color](
		const DirectX::XMFLOAT3& n,
		const DirectX::XMFLOAT3& v0,
		const DirectX::XMFLOAT3& v1,
		const DirectX::XMFLOAT3& v2,
		const DirectX::XMFLOAT3& v3)
	{
		unsigned short base = static_cast<unsigned short>(GetVertices().size());
		AddVertex({ v0, color, n, { 0.0f, 1.0f } });
		AddVertex({ v1, color, n, { 0.0f, 0.0f } });
		AddVertex({ v2, color, n, { 1.0f, 0.0f } });
		AddVertex({ v3, color, n, { 1.0f, 1.0f } });

		AddIndex(static_cast<unsigned short>(base + 0));
		AddIndex(static_cast<unsigned short>(base + 1));
		AddIndex(static_cast<unsigned short>(base + 2));
		AddIndex(static_cast<unsigned short>(base + 0));
		AddIndex(static_cast<unsigned short>(base + 2));
		AddIndex(static_cast<unsigned short>(base + 3));
	};

	addFace({ 0.0f, 0.0f, -1.0f }, { -h, -h, -h }, { -h, h, -h }, { h, h, -h }, { h, -h, -h });
	addFace({ 0.0f, 0.0f, 1.0f }, { h, -h, h }, { h, h, h }, { -h, h, h }, { -h, -h, h });
	addFace({ -1.0f, 0.0f, 0.0f }, { -h, -h, h }, { -h, h, h }, { -h, h, -h }, { -h, -h, -h });
	addFace({ 1.0f, 0.0f, 0.0f }, { h, -h, -h }, { h, h, -h }, { h, h, h }, { h, -h, h });
	addFace({ 0.0f, 1.0f, 0.0f }, { -h, h, -h }, { -h, h, h }, { h, h, h }, { h, h, -h });
	addFace({ 0.0f, -1.0f, 0.0f }, { -h, -h, h }, { -h, -h, -h }, { h, -h, -h }, { h, -h, h });
}

PlaneMesh::PlaneMesh()
{
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float h = kHalfSize;
	const DirectX::XMFLOAT3 normal(0.0f, 1.0f, 0.0f);

	unsigned short base = static_cast<unsigned short>(GetVertices().size());
	AddVertex({ { -h, 0.0f, -h }, color, normal, { 0.0f, 1.0f } });
	AddVertex({ { -h, 0.0f, h }, color, normal, { 0.0f, 0.0f } });
	AddVertex({ { h, 0.0f, h }, color, normal, { 1.0f, 0.0f } });
	AddVertex({ { h, 0.0f, -h }, color, normal, { 1.0f, 1.0f } });

	AddIndex(static_cast<unsigned short>(base + 0));
	AddIndex(static_cast<unsigned short>(base + 1));
	AddIndex(static_cast<unsigned short>(base + 2));
	AddIndex(static_cast<unsigned short>(base + 0));
	AddIndex(static_cast<unsigned short>(base + 2));
	AddIndex(static_cast<unsigned short>(base + 3));
}

SphereMesh::SphereMesh()
{
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float radius = kHalfSize;

	for (int stack = 0; stack <= kDefaultStacks; ++stack)
	{
		float v = static_cast<float>(stack) / static_cast<float>(kDefaultStacks);
		float phi = v * kPi;
		float y = std::cos(phi) * radius;
		float ringRadius = std::sin(phi) * radius;

		for (int slice = 0; slice <= kDefaultSlices; ++slice)
		{
			float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
			float theta = u * kPi * 2.0f;
			float x = std::cos(theta) * ringRadius;
			float z = std::sin(theta) * ringRadius;
			DirectX::XMFLOAT3 normal(x / radius, y / radius, z / radius);

			AddVertex({ { x, y, z }, color, normal, { u, 1.0f - v } });
		}
	}

	for (int stack = 0; stack < kDefaultStacks; ++stack)
	{
		for (int slice = 0; slice < kDefaultSlices; ++slice)
		{
			unsigned short i0 = static_cast<unsigned short>(stack * (kDefaultSlices + 1) + slice);
			unsigned short i1 = static_cast<unsigned short>(i0 + kDefaultSlices + 1);
			unsigned short i2 = static_cast<unsigned short>(i0 + 1);
			unsigned short i3 = static_cast<unsigned short>(i1 + 1);

			AddIndex(i0);
			AddIndex(i1);
			AddIndex(i2);
			AddIndex(i2);
			AddIndex(i1);
			AddIndex(i3);
		}
	}
}

CapsuleMesh::CapsuleMesh()
{
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float radius = 0.25f;
	const float cylinderHalfHeight = 0.25f;
	const float totalHeight = (radius * 2.0f) + (cylinderHalfHeight * 2.0f);
	const float totalHalfHeight = totalHeight * 0.5f;

	std::vector<float> ringY;
	std::vector<float> ringRadius;
	std::vector<float> ringNormalY;

	for (int i = 0; i <= kDefaultHemisphereStacks; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(kDefaultHemisphereStacks);
		float phi = t * (kPi * 0.5f);
		float y = std::cos(phi) * radius + cylinderHalfHeight;
		ringY.push_back(y);
		ringRadius.push_back(std::sin(phi) * radius);
		ringNormalY.push_back(std::cos(phi));
	}

	if (cylinderHalfHeight > 0.0f)
	{
		ringY.push_back(-cylinderHalfHeight);
		ringRadius.push_back(radius);
		ringNormalY.push_back(0.0f);
	}

	for (int i = 1; i <= kDefaultHemisphereStacks; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(kDefaultHemisphereStacks);
		float phi = t * (kPi * 0.5f);
		float y = -std::cos(phi) * radius - cylinderHalfHeight;
		ringY.push_back(y);
		ringRadius.push_back(std::sin(phi) * radius);
		ringNormalY.push_back(-std::cos(phi));
	}

	for (size_t ring = 0; ring < ringY.size(); ++ring)
	{
		for (int slice = 0; slice <= kDefaultSlices; ++slice)
		{
			float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
			float theta = u * kPi * 2.0f;
			float x = std::cos(theta) * ringRadius[ring];
			float z = std::sin(theta) * ringRadius[ring];
			float normalY = ringNormalY[ring];
			DirectX::XMFLOAT3 normal(
				std::cos(theta) * (ringRadius[ring] / radius),
				normalY,
				std::sin(theta) * (ringRadius[ring] / radius));
			float v = 1.0f - ((ringY[ring] + totalHalfHeight) / totalHeight);

			AddVertex({ { x, ringY[ring], z }, color, normal, { u, v } });
		}
	}

	int ringCount = static_cast<int>(ringY.size());
	int stride = kDefaultSlices + 1;
	for (int ring = 0; ring < ringCount - 1; ++ring)
	{
		for (int slice = 0; slice < kDefaultSlices; ++slice)
		{
			unsigned short i0 = static_cast<unsigned short>(ring * stride + slice);
			unsigned short i1 = static_cast<unsigned short>(i0 + stride);
			unsigned short i2 = static_cast<unsigned short>(i0 + 1);
			unsigned short i3 = static_cast<unsigned short>(i1 + 1);

			AddIndex(i0);
			AddIndex(i1);
			AddIndex(i2);
			AddIndex(i2);
			AddIndex(i1);
			AddIndex(i3);
		}
	}
}

CylinderMesh::CylinderMesh()
{
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float radius = kHalfSize;
	const float halfHeight = kHalfSize;

	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		DirectX::XMFLOAT3 normal(std::cos(theta), 0.0f, std::sin(theta));

		AddVertex({ { x, -halfHeight, z }, color, normal, { u, 1.0f } });
		AddVertex({ { x, halfHeight, z }, color, normal, { u, 0.0f } });
	}

	for (int slice = 0; slice < kDefaultSlices; ++slice)
	{
		unsigned short base = static_cast<unsigned short>(slice * 2);
		AddIndex(static_cast<unsigned short>(base + 0));
		AddIndex(static_cast<unsigned short>(base + 1));
		AddIndex(static_cast<unsigned short>(base + 2));
		AddIndex(static_cast<unsigned short>(base + 2));
		AddIndex(static_cast<unsigned short>(base + 1));
		AddIndex(static_cast<unsigned short>(base + 3));
	}

	unsigned short topCenterIndex = static_cast<unsigned short>(GetVertices().size());
	AddVertex({ { 0.0f, halfHeight, 0.0f }, color, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f } });
	unsigned short bottomCenterIndex = static_cast<unsigned short>(GetVertices().size());
	AddVertex({ { 0.0f, -halfHeight, 0.0f }, color, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f } });

	unsigned short topRingStart = static_cast<unsigned short>(GetVertices().size());
	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		float tu = (x / (radius * 2.0f)) + 0.5f;
		float tv = (z / (radius * 2.0f)) + 0.5f;

		AddVertex({ { x, halfHeight, z }, color, { 0.0f, 1.0f, 0.0f }, { tu, tv } });
	}

	unsigned short bottomRingStart = static_cast<unsigned short>(GetVertices().size());
	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		float tu = (x / (radius * 2.0f)) + 0.5f;
		float tv = (z / (radius * 2.0f)) + 0.5f;

		AddVertex({ { x, -halfHeight, z }, color, { 0.0f, -1.0f, 0.0f }, { tu, tv } });
	}

	for (int slice = 0; slice < kDefaultSlices; ++slice)
	{
		AddIndex(topCenterIndex);
		AddIndex(static_cast<unsigned short>(topRingStart + slice + 1));
		AddIndex(static_cast<unsigned short>(topRingStart + slice));

		AddIndex(bottomCenterIndex);
		AddIndex(static_cast<unsigned short>(bottomRingStart + slice));
		AddIndex(static_cast<unsigned short>(bottomRingStart + slice + 1));
	}
}
