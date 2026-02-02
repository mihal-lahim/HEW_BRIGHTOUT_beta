#include "ConstantBuffer.h"
#include "GraphicsDevice.h"
#include <cstring>

void ConstantBuffer::CreateBuffers(GraphicsDevice* device, UsageType type, int size)
{
	if (!device) {
		return;
	}

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (type == UsageType::DYNAMIC) {
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else {
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	device->GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.GetAddressOf());
}

void ConstantBuffer::UpdateBuffer(GraphicsDevice* device, UsageType type, const void* data, int size)
{
	if (!device || !data) {
		return;
	}

	if (type == UsageType::DYNAMIC) {
		D3D11_MAPPED_SUBRESOURCE mapped{};
		if (SUCCEEDED(device->GetDeviceContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
			memcpy(mapped.pData, data, size);
			device->GetDeviceContext()->Unmap(m_constantBuffer.Get(), 0);
		}
	} else {
		device->GetDeviceContext()->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, data, 0, 0);
	}
}

void PerFrameCB::CreateBuffers(GraphicsDevice& device)
{
	ConstantBuffer::CreateBuffers(&device, m_usageType, sizeof(PerFrameCB::CBData));
}

void PerFrameCB::UpdateBuffer(GraphicsDevice& device, const void* data)
{
	ConstantBuffer::UpdateBuffer(&device, m_usageType, data, sizeof(PerFrameCB::CBData));

	ID3D11Buffer* buffer = m_constantBuffer.Get();
	device.GetDeviceContext()->VSSetConstantBuffers(2, 1, &buffer);
}

void PerCameraCB::CreateBuffers(GraphicsDevice& device)
{
	ConstantBuffer::CreateBuffers(&device, m_usageType, sizeof(PerCameraCB::CBData));
}

void PerCameraCB::UpdateBuffer(GraphicsDevice& device, const void* data)
{
	ConstantBuffer::UpdateBuffer(&device, m_usageType, data, sizeof(PerCameraCB::CBData));

	ID3D11Buffer* buffer = m_constantBuffer.Get();
	device.GetDeviceContext()->VSSetConstantBuffers(1, 1, &buffer);
}

void PerObjectCB::CreateBuffers(GraphicsDevice& device)
{
	ConstantBuffer::CreateBuffers(&device, m_usageType, sizeof(PerObjectCB::CBData));
}

void PerObjectCB::UpdateBuffer(GraphicsDevice& device, const void* data)
{
	ConstantBuffer::UpdateBuffer(&device, m_usageType, data, sizeof(PerObjectCB::CBData));

	ID3D11Buffer* buffer = m_constantBuffer.Get();
	device.GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);
}
