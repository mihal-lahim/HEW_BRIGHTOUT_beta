#ifndef CONSTANT_BUFFER_INL
#define CONSTANT_BUFFER_INL


template<typename T>
bool ConstantBuffer<T>::CreateBuffer(GraphicsDevice& device)
{
	// バッファ設定
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(T);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// 使用方法による条件分岐
	if (m_usageType == CBUsageType::DYNAMIC) {
		// DYNAMICの場合
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else {
		// DEFAULTの場合
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0;
	}

	// バッファの生成
	return SUCCEEDED(device.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.GetAddressOf()));
}

template<typename T>
void ConstantBuffer<T>::UpdateBuffer(GraphicsDevice& device, const T& data)
{

	// 使用方法による条件分岐
	if (m_usageType == CBUsageType::DYNAMIC) {

		// DYNAMICの場合はMapしつつDISCARDで更新
		D3D11_MAPPED_SUBRESOURCE mapped{};
		if (SUCCEEDED(device.GetDeviceContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
			memcpy(mapped.pData, &data, sizeof(T));
			device.GetDeviceContext()->Unmap(m_constantBuffer.Get(), 0);
		}
	}
	else {
		// DEFAULTの場合はUpdateSubresourceで更新
		device.GetDeviceContext()->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &data, 0, 0);
	}
}

template<typename T>
void ConstantBuffer<T>::Bind(GraphicsDevice& device, ShaderType type, UINT slot)
{
	// バッファの取得
	ID3D11Buffer* buffer = m_constantBuffer.Get();
	if (!buffer) {
		return;
	}
	// デバイスコンテキストの取得
	auto* context = device.GetDeviceContext();

	// シェーダーステージによる条件分岐
	switch (type) {
	case ShaderType::Vertex:
		context->VSSetConstantBuffers(slot, 1, &buffer);
		break;
	case ShaderType::Pixel:
		context->PSSetConstantBuffers(slot, 1, &buffer);
		break;
	}
}



#endif
