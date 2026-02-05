#ifndef MESH_INL
#define MESH_INL

#include <cmath>

namespace
{
	constexpr float kHalfSize = 0.5f;
	constexpr int kDefaultSlices = 16;
	constexpr int kDefaultStacks = 16;
	constexpr int kDefaultHemisphereStacks = 8;

	constexpr float kPi = 3.14159265358979323846f;

	template<typename VA>
	VA MakeVertex(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT4& color,
		const DirectX::XMFLOAT3& normal,
		const DirectX::XMFLOAT2& uv)
	{
		VA vertex{};
		if constexpr (requires(VA v) { v.position; })
		{
			vertex.position = position;
		}
		if constexpr (requires(VA v) { v.color; })
		{
			vertex.color = color;
		}
		if constexpr (requires(VA v) { v.normal; })
		{
			vertex.normal = normal;
		}
		if constexpr (requires(VA v) { v.uv; })
		{
			vertex.uv = uv;
		}
		return vertex;
	}
}


template<typename VA>
bool Mesh::CreateBuffer(GraphicsDevice& device, const std::vector<VA>& vertexes, const std::vector<unsigned short>& indexes)
{
	if (vertexes.empty() || indexes.empty())
	{
		return false;
	}

	m_vertexStride = static_cast<UINT>(sizeof(VA));
	m_vertexCount = static_cast<UINT>(vertexes.size());
	m_indexCount = static_cast<UINT>(indexes.size());

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(VA) * vertexes.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	// 頂点バッファへ流し込むデータの設定
	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertexes.data();

	HRESULT result_v = device.GetDevice()->CreateBuffer(&bd, &sd, m_vertexBuffer.GetAddressOf());
	if (FAILED(result_v)) {
		return false;
	}
	// インデックスバッファ生成
	bd.ByteWidth = static_cast<UINT>(sizeof(unsigned short) * indexes.size());
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// インデックスバッファへ流し込むデータの設定
	sd.pSysMem = indexes.data();
	HRESULT result_i = device.GetDevice()->CreateBuffer(&bd, &sd, m_indexBuffer.GetAddressOf());
	if (FAILED(result_i)) {
		return false;
	}
	return true;
}


inline void Mesh::Render(GraphicsDevice& device)
{
	ID3D11DeviceContext* context = device.GetDeviceContext();
	// 頂点バッファの設定
	UINT stride = m_vertexStride;
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// インデックスバッファの設定
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	// プリミティブトポロジーの設定（三角形リスト）
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 描画
	context->DrawIndexed(m_indexCount, 0, 0);
}

template<typename VS>
template<typename VA>
std::vector<VA> CubeMesh<VS>::CreateVertexes()
{
	std::vector<VA> vertices;
	vertices.reserve(24);
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float h = kHalfSize;

	auto addFace = [&vertices, &color](
		const DirectX::XMFLOAT3& n,
		const DirectX::XMFLOAT3& v0,
		const DirectX::XMFLOAT3& v1,
		const DirectX::XMFLOAT3& v2,
		const DirectX::XMFLOAT3& v3)
		{
			vertices.push_back(MakeVertex<VA>(v0, color, n, { 0.0f, 1.0f }));
			vertices.push_back(MakeVertex<VA>(v1, color, n, { 0.0f, 0.0f }));
			vertices.push_back(MakeVertex<VA>(v2, color, n, { 1.0f, 0.0f }));
			vertices.push_back(MakeVertex<VA>(v3, color, n, { 1.0f, 1.0f }));
		};

	addFace({ 0.0f, 0.0f, -1.0f }, { -h, -h, -h }, { -h, h, -h }, { h, h, -h }, { h, -h, -h });
	addFace({ 0.0f, 0.0f, 1.0f }, { h, -h, h }, { h, h, h }, { -h, h, h }, { -h, -h, h });
	addFace({ -1.0f, 0.0f, 0.0f }, { -h, -h, h }, { -h, h, h }, { -h, h, -h }, { -h, -h, -h });
	addFace({ 1.0f, 0.0f, 0.0f }, { h, -h, -h }, { h, h, -h }, { h, h, h }, { h, -h, h });
	addFace({ 0.0f, 1.0f, 0.0f }, { -h, h, -h }, { -h, h, h }, { h, h, h }, { h, h, -h });
	addFace({ 0.0f, -1.0f, 0.0f }, { -h, -h, h }, { -h, -h, -h }, { h, -h, -h }, { h, -h, h });

	return vertices;
}

template<typename VS>
std::vector<unsigned short> CubeMesh<VS>::CreateIndexes()
{
	std::vector<unsigned short> indexes;
	indexes.reserve(36);
	unsigned short base = 0;
	for (int face = 0; face < 6; ++face)
	{
		indexes.push_back(static_cast<unsigned short>(base + 0));
		indexes.push_back(static_cast<unsigned short>(base + 1));
		indexes.push_back(static_cast<unsigned short>(base + 2));
		indexes.push_back(static_cast<unsigned short>(base + 0));
		indexes.push_back(static_cast<unsigned short>(base + 2));
		indexes.push_back(static_cast<unsigned short>(base + 3));
		base = static_cast<unsigned short>(base + 4);
	}

	return indexes;
}

template<typename VS>
template<typename VA>
std::vector<VA> PlaneMesh<VS>::CreateVertexes()
{
	std::vector<VA> vertices;
	vertices.reserve(4);
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float h = kHalfSize;
	const DirectX::XMFLOAT3 normal(0.0f, 1.0f, 0.0f);

	vertices.push_back(MakeVertex<VA>({ -h, 0.0f, -h }, color, normal, { 0.0f, 1.0f }));
	vertices.push_back(MakeVertex<VA>({ -h, 0.0f, h }, color, normal, { 0.0f, 0.0f }));
	vertices.push_back(MakeVertex<VA>({ h, 0.0f, h }, color, normal, { 1.0f, 0.0f }));
	vertices.push_back(MakeVertex<VA>({ h, 0.0f, -h }, color, normal, { 1.0f, 1.0f }));

	return vertices;
}

template<typename VS>
std::vector<unsigned short> PlaneMesh<VS>::CreateIndexes()
{
	return { 0, 1, 2, 0, 2, 3 };
}

template<typename VS>
template<typename VA>
std::vector<VA> SphereMesh<VS>::CreateVertexes()
{
	std::vector<VA> vertices;
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float radius = kHalfSize;

	vertices.reserve((kDefaultStacks + 1) * (kDefaultSlices + 1));
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

			vertices.push_back(MakeVertex<VA>({ x, y, z }, color, normal, { u, 1.0f - v }));
		}
	}

	return vertices;
}

template<typename VS>
std::vector<unsigned short> SphereMesh<VS>::CreateIndexes()
{
	std::vector<unsigned short> indexes;
	indexes.reserve(kDefaultStacks * kDefaultSlices * 6);
	for (int stack = 0; stack < kDefaultStacks; ++stack)
	{
		for (int slice = 0; slice < kDefaultSlices; ++slice)
		{
			unsigned short i0 = static_cast<unsigned short>(stack * (kDefaultSlices + 1) + slice);
			unsigned short i1 = static_cast<unsigned short>(i0 + kDefaultSlices + 1);
			unsigned short i2 = static_cast<unsigned short>(i0 + 1);
			unsigned short i3 = static_cast<unsigned short>(i1 + 1);

			indexes.push_back(i0);
			indexes.push_back(i1);
			indexes.push_back(i2);
			indexes.push_back(i2);
			indexes.push_back(i1);
			indexes.push_back(i3);
		}
	}

	return indexes;
}

template<typename VS>
template<typename VA>
std::vector<VA> CapsuleMesh<VS>::CreateVertexes()
{
	std::vector<VA> vertices;
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

	vertices.reserve(ringY.size() * static_cast<size_t>(kDefaultSlices + 1));
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

			vertices.push_back(MakeVertex<VA>({ x, ringY[ring], z }, color, normal, { u, v }));
		}
	}

	return vertices;
}

template<typename VS>
std::vector<unsigned short> CapsuleMesh<VS>::CreateIndexes()
{
	std::vector<unsigned short> indexes;
	std::vector<float> ringY;
	const float radius = 0.25f;
	const float cylinderHalfHeight = 0.25f;

	for (int i = 0; i <= kDefaultHemisphereStacks; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(kDefaultHemisphereStacks);
		float phi = t * (kPi * 0.5f);
		float y = std::cos(phi) * radius + cylinderHalfHeight;
		ringY.push_back(y);
	}

	if (cylinderHalfHeight > 0.0f)
	{
		ringY.push_back(-cylinderHalfHeight);
	}

	for (int i = 1; i <= kDefaultHemisphereStacks; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(kDefaultHemisphereStacks);
		float phi = t * (kPi * 0.5f);
		float y = -std::cos(phi) * radius - cylinderHalfHeight;
		ringY.push_back(y);
	}

	int ringCount = static_cast<int>(ringY.size());
	int stride = kDefaultSlices + 1;
	indexes.reserve(static_cast<size_t>((ringCount - 1) * kDefaultSlices * 6));
	for (int ring = 0; ring < ringCount - 1; ++ring)
	{
		for (int slice = 0; slice < kDefaultSlices; ++slice)
		{
			unsigned short i0 = static_cast<unsigned short>(ring * stride + slice);
			unsigned short i1 = static_cast<unsigned short>(i0 + stride);
			unsigned short i2 = static_cast<unsigned short>(i0 + 1);
			unsigned short i3 = static_cast<unsigned short>(i1 + 1);

			indexes.push_back(i0);
			indexes.push_back(i1);
			indexes.push_back(i2);
			indexes.push_back(i2);
			indexes.push_back(i1);
			indexes.push_back(i3);
		}
	}

	return indexes;
}

template<typename VS>
template<typename VA>
std::vector<VA> CylinderMesh<VS>::CreateVertexes()
{
	std::vector<VA> vertices;
	const DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	const float radius = kHalfSize;
	const float halfHeight = kHalfSize;

	vertices.reserve(static_cast<size_t>((kDefaultSlices + 1) * 2 + 2 + (kDefaultSlices + 1) * 2));
	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		DirectX::XMFLOAT3 normal(std::cos(theta), 0.0f, std::sin(theta));

		vertices.push_back(MakeVertex<VA>({ x, -halfHeight, z }, color, normal, { u, 1.0f }));
		vertices.push_back(MakeVertex<VA>({ x, halfHeight, z }, color, normal, { u, 0.0f }));
	}

	vertices.push_back(MakeVertex<VA>({ 0.0f, halfHeight, 0.0f }, color, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f }));
	vertices.push_back(MakeVertex<VA>({ 0.0f, -halfHeight, 0.0f }, color, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f }));

	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		float tu = (x / (radius * 2.0f)) + 0.5f;
		float tv = (z / (radius * 2.0f)) + 0.5f;

		vertices.push_back(MakeVertex<VA>({ x, halfHeight, z }, color, { 0.0f, 1.0f, 0.0f }, { tu, tv }));
	}

	for (int slice = 0; slice <= kDefaultSlices; ++slice)
	{
		float u = static_cast<float>(slice) / static_cast<float>(kDefaultSlices);
		float theta = u * kPi * 2.0f;
		float x = std::cos(theta) * radius;
		float z = std::sin(theta) * radius;
		float tu = (x / (radius * 2.0f)) + 0.5f;
		float tv = (z / (radius * 2.0f)) + 0.5f;

		vertices.push_back(MakeVertex<VA>({ x, -halfHeight, z }, color, { 0.0f, -1.0f, 0.0f }, { tu, tv }));
	}

	return vertices;
}

template<typename VS>
std::vector<unsigned short> CylinderMesh<VS>::CreateIndexes()
{
	std::vector<unsigned short> indexes;
	indexes.reserve(static_cast<size_t>(kDefaultSlices * 12));

	for (int slice = 0; slice < kDefaultSlices; ++slice)
	{
		unsigned short base = static_cast<unsigned short>(slice * 2);
		indexes.push_back(static_cast<unsigned short>(base + 0));
		indexes.push_back(static_cast<unsigned short>(base + 1));
		indexes.push_back(static_cast<unsigned short>(base + 2));
		indexes.push_back(static_cast<unsigned short>(base + 2));
		indexes.push_back(static_cast<unsigned short>(base + 1));
		indexes.push_back(static_cast<unsigned short>(base + 3));
	}

	unsigned short topCenterIndex = static_cast<unsigned short>((kDefaultSlices + 1) * 2);
	unsigned short bottomCenterIndex = static_cast<unsigned short>(topCenterIndex + 1);
	unsigned short topRingStart = static_cast<unsigned short>(bottomCenterIndex + 1);
	unsigned short bottomRingStart = static_cast<unsigned short>(topRingStart + (kDefaultSlices + 1));

	for (int slice = 0; slice < kDefaultSlices; ++slice)
	{
		indexes.push_back(topCenterIndex);
		indexes.push_back(static_cast<unsigned short>(topRingStart + slice + 1));
		indexes.push_back(static_cast<unsigned short>(topRingStart + slice));

		indexes.push_back(bottomCenterIndex);
		indexes.push_back(static_cast<unsigned short>(bottomRingStart + slice));
		indexes.push_back(static_cast<unsigned short>(bottomRingStart + slice + 1));
	}

	return indexes;
}




#endif
