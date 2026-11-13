#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H


#include "GraphicsDevice.h"
#include <wrl/client.h>
#include <d3d11.h>


// 定数バッファクラス
class ConstantBuffer
{
public:
	// バッファの作成、更新
	bool CreateBuffer(GraphicsDevice& device, UINT size);
	void UpdateBuffer(GraphicsDevice& device, const void* data, UINT size) const;

	// バインド
	void BindVS(GraphicsDevice& device, UINT slot) const;
	void BindPS(GraphicsDevice& device, UINT slot) const;

private:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
};



#endif
