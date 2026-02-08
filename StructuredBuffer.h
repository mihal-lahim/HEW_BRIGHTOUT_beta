#ifndef STRUCTURED_BUFFER_H
#define STRUCTURED_BUFFER_H

#include "GraphicsDevice.h"
#include <d3d11.h>
#include <wrl/client.h>

// バッファの使用方法
enum class USAGE_TYPE
{
	NONE,
	DEFAULT,
	DYNAMIC,
};

// ビューの種類
enum class VIEW_TYPE
{
	NONE,
	SRV,
	UAV,
};


class StructuredBuffer
{
public:
	// バッファの作成、更新、バインド
	bool CreateBuffer(GraphicsDevice& device, UINT count, UINT size, USAGE_TYPE usage, VIEW_TYPE view);
	void UpdateBuffer(GraphicsDevice& device, const void* data);
	void BindVS(GraphicsDevice& device, UINT slot);
	void BindPS(GraphicsDevice& device, UINT slot);
	void BindCS(GraphicsDevice& device, UINT slot);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav = nullptr;
	UINT m_elementCount = 0;
	UINT m_elementSize = 0;

	USAGE_TYPE m_usageType = USAGE_TYPE::NONE;
	VIEW_TYPE m_viewType = VIEW_TYPE::NONE;
};


#endif
