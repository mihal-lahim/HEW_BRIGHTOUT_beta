#ifndef MATERIAL_H
#define MATERIAL_H

#include "Resource.h"
#include "GraphicsDevice.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include <unordered_map>
#include <string>
#include <DirectXMath.h>


class Material : public Resource
{
public:
	// 使用シェーダープログラムの設定と定数バッファの作成
	bool CreateBuffer(GraphicsDevice& device, ShaderProgram* shaderProgram);
	// マテリアルの適用
	void Apply(GraphicsDevice& device);
	// 定数バッファのバインド
	void Bind(GraphicsDevice& device);

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


private:
	// マテリアルパラメータ
	std::unordered_map<std::string, float> m_floatParams;
	std::unordered_map<std::string, DirectX::XMFLOAT4> m_float4Params;
	std::unordered_map<std::string, DirectX::XMMATRIX> m_matrix4x4Params;

	// 使用シェーダープログラム
	ShaderProgram* m_shaderProgram = nullptr;

	// マテリアル用定数バッファ
	ConstantBuffer materialConstantBuffer = {};

	// 変更フラグ
	bool m_isDirty = true;

	friend class RenderingSystem;
};


#endif
