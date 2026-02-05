/*==============================================================================

   シェーダー [shader.h]
														 Author : Rintarou Sumi
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER_H
#define	SHADER_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "GraphicsDevice.h"
#include "Resource.h"
#include "ConstantBuffer.h"


// シェーダーステージ種別
enum class ShaderType
{
	None,
	Vertex,
	Pixel,
};

// 定数バッファの使用方法
enum class CBUsageType
{
	DEFAULT,
	DYNAMIC,
};

// シェーダーの基底クラス
class Shader : public Resource
{
public:
	// 毎フレーム用の定数バッファ構造体
	struct PerFrameCB
	{
		static constexpr CBUsageType USAGE_TYPE = CBUsageType::DEFAULT;

		DirectX::XMFLOAT4 ambientColor = {};
		DirectX::XMFLOAT3 directionalLightVector = {};
		float padding = 0.0f;
		DirectX::XMFLOAT4 directionalLightColor = {};
	};

	// 毎カメラ用の定数バッファ構造体
	struct PerCameraCB
	{
		static constexpr CBUsageType USAGE_TYPE = CBUsageType::DEFAULT;

		DirectX::XMMATRIX viewMatrix = {};
		DirectX::XMMATRIX projectionMatrix = {};
	};

	// 毎オブジェクト用の定数バッファ構造体
	struct PerObjectCB
	{
		static constexpr CBUsageType USAGE_TYPE = CBUsageType::DYNAMIC;

		DirectX::XMMATRIX worldMatrix = {};
	};


	Shader(ShaderType type)
		: m_type(type)
	{
	}
	virtual ~Shader() = default;

	// バッファの作成とバインド
	virtual bool CreateBuffer(GraphicsDevice& device) = 0;
	virtual void Bind(GraphicsDevice& device) = 0;
protected:
	// シェーダーステージ種別
	ShaderType m_type = ShaderType::None;

	// シェーダー本体
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	// コンパイル済みシェーダーファイルの読み込み
	bool ReadCompiledShader(const char* path, Microsoft::WRL::ComPtr<ID3DBlob>& blob);

	// シェーダーの作成
	bool CreateShaderFromFile(GraphicsDevice& device, const char* path, Microsoft::WRL::ComPtr<ID3DBlob>* shaderBlob = nullptr);
};



// 頂点シェーダーの基底クラス
class VertexShader : public Shader
{
public:
	VertexShader()
		: Shader(ShaderType::Vertex)
	{
	}
	virtual ~VertexShader() = default;

protected:
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 頂点入力要素のフォーマットを取得
	DXGI_FORMAT GetInputElementFormat(const D3D11_SIGNATURE_PARAMETER_DESC& desc);

	// 入力レイアウトの作成
	bool CreateInputLayoutFromBlob(GraphicsDevice& device, ID3DBlob* blob, Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout);

	// 頂点シェーダーと入力レイアウトの作成
	bool CreateVertexShaderWithLayout(GraphicsDevice& device, const char* path);
};

// ピクセルシェーダーの基底クラス
class PixelShader : public Shader
{
public:
	PixelShader()
		: Shader(ShaderType::Pixel)
	{
	}
	virtual ~PixelShader() = default;
};

// 2D用頂点シェーダーとピクセルシェーダーの基底クラス
class VertexShader2D : public VertexShader
{
public:
	VertexShader2D()
		: VertexShader()
	{
	}
	virtual ~VertexShader2D() = default;
};

class PixelShader2D : public PixelShader
{
public:
	PixelShader2D()
		: PixelShader()
	{
	}
	virtual ~PixelShader2D() = default;
};

// 3D用頂点シェーダーとピクセルシェーダーの基底クラス
class VertexShader3D : public VertexShader
{
public:
	VertexShader3D()
		: VertexShader()
	{
	}
	virtual ~VertexShader3D() = default;
};

class PixelShader3D : public PixelShader
{
public:
	PixelShader3D()
		: PixelShader()
	{
	}
	virtual ~PixelShader3D() = default;
};

// スプライト用頂点シェーダー(2D)
class SpriteVS : public VertexShader2D
{
public:
	struct VertexAttribute
	{
		DirectX::XMFLOAT2 position = {};
		DirectX::XMFLOAT4 color = {};
		DirectX::XMFLOAT2 uv = {};
	};

	SpriteVS()
		: VertexShader2D()
	{
	}
	virtual bool CreateBuffer(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

// スプライト用頂点シェーダー(2D)
class SpritePS : public PixelShader2D
{
public:
	SpritePS()
		: PixelShader2D()
	{
	}
	// マテリアル用定数バッファの構造体
	struct PerMaterialCB
	{
		static constexpr CBUsageType USAGE_TYPE = CBUsageType::DYNAMIC;

		DirectX::XMFLOAT4 color = {};
	};


	// Material用の設定データを作成
	virtual PerMaterialCB* CreateMaterialCB()
	{
		return new PerMaterialCB;
	}

	// バッファの作成とバインド
	virtual bool CreateBuffer(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

// メッシュ用頂点シェーダー(3D)
class MeshVS : public VertexShader3D
{
public:
	struct VertexAttribute
	{
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT4 color = {};
		DirectX::XMFLOAT3 normal = {};
		DirectX::XMFLOAT2 uv = {};
	};

	MeshVS()
		: VertexShader3D()
	{
	}
	// バッファの作成とバインド
	virtual bool CreateBuffer(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

// メッシュ用ピクセルシェーダー(3D)
class MeshPS : public PixelShader3D
{
public:
	MeshPS()
		: PixelShader3D()
	{
	}
	struct PerMaterialCB
	{
		static constexpr CBUsageType USAGE_TYPE = CBUsageType::DYNAMIC;

		DirectX::XMFLOAT4 color = {};
	};

	// Material用の設定データを作成
	PerMaterialCB* CreateMaterialCB()
	{
		return new PerMaterialCB;
	}

	// バッファの作成とバインド
	virtual bool CreateBuffer(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

#endif
