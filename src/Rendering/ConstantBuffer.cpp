#include "ConstantBuffer.h"

bool ConstantBuffer::CreateBuffer(GraphicsDevice& device, UINT size)
{
	// 16バイトアライメントに調整
	UINT bufferSize = (size + 15) & ~15;

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = bufferSize;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device.GetDevice()->CreateBuffer(&desc, nullptr, m_constantBuffer.GetAddressOf());

	return SUCCEEDED(hr);
}

void ConstantBuffer::UpdateBuffer(GraphicsDevice& device, const void* data, UINT size) const
{
	D3D11_MAPPED_SUBRESOURCE mapped{};
	device.GetDeviceContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	std::memcpy(mapped.pData, data, size);

	device.GetDeviceContext()->Unmap(m_constantBuffer.Get(), 0);

}

void ConstantBuffer::BindVS(GraphicsDevice& device, UINT slot) const
{
	device.GetDeviceContext()->VSSetConstantBuffers(slot, 1, m_constantBuffer.GetAddressOf());
}

void ConstantBuffer::BindPS(GraphicsDevice& device, UINT slot) const
{
	device.GetDeviceContext()->PSSetConstantBuffers(slot, 1, m_constantBuffer.GetAddressOf());
}
