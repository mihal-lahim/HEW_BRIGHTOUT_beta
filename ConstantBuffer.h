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
	void UpdateBuffer(GraphicsDevice& device, const void* data, UINT size);

	// バインド
	void BindVS(GraphicsDevice& device, UINT slot);
	void BindPS(GraphicsDevice& device, UINT slot);

private:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
};



#endif
