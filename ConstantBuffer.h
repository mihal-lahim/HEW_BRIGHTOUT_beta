#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include "Resource.h"
#include <wrl/client.h>
#include <d3d11.h>


// シェーダーステージ種別(前方宣言)
enum class ShaderType;

// 定数バッファの使用方法(前方宣言)
enum class CBUsageType;


// 定数バッファの基底クラス
class ConstantBufferBase : public Resource
{
};

// 定数バッファクラス
template<typename T>
class ConstantBuffer : public ConstantBufferBase
{
public:
	ConstantBuffer() = default;
	virtual ~ConstantBuffer() = default;

	// バッファの作成と更新、バインド
	virtual bool CreateBuffer(GraphicsDevice& device);
	virtual void UpdateBuffer(GraphicsDevice& device, const T& data);
	void Bind(GraphicsDevice& device, ShaderType type, UINT slot);

	// バッファの取得
	virtual ID3D11Buffer* GetBuffer() const 
	{ 
		return m_constantBuffer.Get();
	}

protected:
	// 定数バッファの使用方法
	CBUsageType m_usageType = T::USAGE_TYPE;

	// 定数バッファ本体
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;
};

#include "ConstantBuffer.inl"

#endif
