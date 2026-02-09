#include "StructuredBuffer.h"
#include "DebugOstream.h"

bool StructuredBuffer::CreateBuffer(GraphicsDevice& device, UINT count, UINT size, USAGE_TYPE usage, VIEW_TYPE view)
{
	m_usageType = usage;
	m_viewType = view;
	m_elementCount = count;
	m_elementSize = size;

	// バッファの情報を設定
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = count * size;
	bufferDesc.StructureByteStride = size;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// ビューに応じたバインド設定
	switch (view)
	{
	case VIEW_TYPE::SRV:
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		break;
	case VIEW_TYPE::UAV:
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		break;
	default:
		return false;
	}

	// 使用方法の設定
	switch (usage)
	{
	case USAGE_TYPE::DEFAULT:
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
		break;
	case USAGE_TYPE::DYNAMIC:
		if (view == VIEW_TYPE::UAV)
		{
			hal::dout << "StructuredBuffer::CreateBuffer: UAV cannot be DYNAMIC usage." << std::endl;
			return false;
		}
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	default:
		return false;
	}

	// バッファの作成
	const HRESULT hr = device.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		hal::dout << "StructuredBuffer::CreateBuffer failed. hr=0x" << std::hex << hr << std::dec
			<< " count=" << count << " size=" << size
			<< " usage=" << static_cast<int>(usage) << " view=" << static_cast<int>(view) << std::endl;
		return false;
	}

	// ビューの作成
	if (view == VIEW_TYPE::SRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementWidth = count;
		if (FAILED(device.GetDevice()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_srv.GetAddressOf())))
		{
			return false;
		}
	}
	else if (view == VIEW_TYPE::UAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = count;
		if (FAILED(device.GetDevice()->CreateUnorderedAccessView(m_buffer.Get(), &uavDesc, m_uav.GetAddressOf())))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void StructuredBuffer::UpdateBuffer(GraphicsDevice& device, const void* data)
{
	switch (m_usageType)
	{
	case USAGE_TYPE::DEFAULT:
		device.GetDeviceContext()->UpdateSubresource(m_buffer.Get(), 0, nullptr, data, 0, 0);
		break;
	case USAGE_TYPE::DYNAMIC:
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		if (SUCCEEDED(device.GetDeviceContext()->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			memcpy(mappedResource.pData, data, m_elementCount * m_elementSize);
			device.GetDeviceContext()->Unmap(m_buffer.Get(), 0);
		}
		break;
	}
}

void StructuredBuffer::BindVS(GraphicsDevice& device, UINT slot)
{
	switch (m_usageType)
	{
	case USAGE_TYPE::DEFAULT:
		device.GetDeviceContext()->VSSetShaderResources(slot, 1, m_srv.GetAddressOf());
		break;
	case USAGE_TYPE::DYNAMIC:
		device.GetDeviceContext()->VSSetShaderResources(slot, 1, m_srv.GetAddressOf());
		break;
	}
}

void StructuredBuffer::BindPS(GraphicsDevice& device, UINT slot)
{
	switch (m_usageType)
	{
	case USAGE_TYPE::DEFAULT:
		device.GetDeviceContext()->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
		break;
	case USAGE_TYPE::DYNAMIC:
		device.GetDeviceContext()->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
		break;
	}
}

void StructuredBuffer::BindCS(GraphicsDevice& device, UINT slot)
{
	device.GetDeviceContext()->CSSetUnorderedAccessViews(slot, 1, m_uav.GetAddressOf(), nullptr);
}
