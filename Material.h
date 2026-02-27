#ifndef MATERIAL_H
#define MATERIAL_H

#include "GraphicsDevice.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include <unordered_map>
#include <string>
#include <DirectXMath.h>


class Material
{
public:

	Material()
	{
		SetColor({ 1.0f,1.0f,1.0f,1.0f });
		SetFloat4("uv_rect", { 0.0f, 0.0f, 1.0f, 1.0f });
	}

	// 使用シェーダープログラムの設定と定数バッファの作成
	bool CreateBuffer(GraphicsDevice& device, ShaderProgram* _shaderProgram);
	// マテリアルの適用
	void Apply(GraphicsDevice& device) const;
	// 定数バッファのバインド
	void Bind(GraphicsDevice& device) const;

	// パラメータの設定
	void SetFloat(const std::string& name, float value)
	{
		m_floatParams[name] = value;
		m_isDirty = true;
	}
	void SetFloat4(const std::string& name, const DirectX::XMFLOAT4& value)
	{
		m_float4Params[name] = value;
		m_isDirty = true;
	}
	void SetMatrix(const std::string& name, const DirectX::XMMATRIX& value)
	{
		m_matrix4x4Params[name] = value;
		m_isDirty = true;
	}


	// ショートカット
	void SetColor(const DirectX::XMFLOAT4& color)
	{
		SetFloat4("diffuse_color", color);
	}

	// テクスチャ
	const Texture* texture = nullptr;

	// 遅延ロード用テクスチャパス
	std::wstring texturePath = L"texture/Default.png";

	// 遅延ロード用シェーダーパス
	std::string vsPath = "MeshVS.cso";
	std::string psPath = "MeshPS.cso";

	// 使用シェーダープログラム
	ShaderProgram* shaderProgram = nullptr;


private:
	// マテリアルパラメータ
	std::unordered_map<std::string, float> m_floatParams;
	std::unordered_map<std::string, DirectX::XMFLOAT4> m_float4Params;
	std::unordered_map<std::string, DirectX::XMMATRIX> m_matrix4x4Params;

	// マテリアル用定数バッファ
	ConstantBuffer materialConstantBuffer = {};

	// 変更フラグ
	mutable bool m_isDirty = true;

	friend class RenderingSystem;
};


#endif
